# Stride Callable Syntax & Desugaring Specification

This document specifies the compact **Stream-Signature Header** and **Unified `code:` Property** syntax for Stride callables (`module`, `reaction`, and `loop`), alongside its relationship to the canonical desugared representation (`ports:`, `entities:`, and `streams:`).

---

## 1. Design Goals & Core Invariants

1. **Preserve the `{ property: value }` Entity Body Invariant:**
   Every entity declaration in Stride (`signal`, `buffer`, `module`, `reaction`, `loop`, `functionTest`) strictly maintains a body composed of `<propertyName>: <propertyValue>` pairs. Raw declarations or streams are never placed loose inside `{ ... }`.
2. **Eliminate Port-to-Entity Triplication:**
   Callables can declare their main inputs, property ports, and main outputs directly in the header using stream invocation syntax (`In >> Name(prop: ...) >> Out`), eliminating redundant `ports:` declarations.
3. **Standardize Terminology (`entities:` instead of `blocks:`):**
   Aligns callable schemas with the *Stride Language Specification* by using `entities:` (for internal declarations) and `entity:` (for port bindings) rather than the legacy `blocks:` / `block:` terminology.
4. **Support Both Sugar and Canonical Forms (Mutually Exclusive):**
   Developers and compiler tests can write either the compact syntax or the explicit canonical syntax. Mutual exclusivity is enforced via semantic validation rules during desugaring / AST validation (providing precise diagnostic messages).

---

## 2. Stream-Signature Header Syntax

A callable header mirrors the exact syntax used to invoke it in a stream:

```stride
<callableType> <MainInputs> >> <CallableName>(<PropertyPorts>) >> <MainOutputs> {
    <properties...>
}
```

* **`module`**: Supports optional `<MainInputs> >>`, `<CallableName>(<PropertyPorts>)`, and `>> <MainOutputs>`.
* **`reaction`**: Triggered by switches/events rather than a main input stream; omits `<MainInputs> >>` (`reaction Name(...) >> <MainOutputs>`).
* **`loop`**: Supports `<MainInputs> >> <LoopName>(<PropertyPorts>) >> <MainOutputs>` in either **Iterator Mode** or **Short-Circuit Mode**.

### 2.1 Port Declaration Rules (Explicit Entity Type for Overrides)

Every port slot in the header (`MainInputs`, `PropertyPorts`, `MainOutputs`) follows two progressive rules:

| Port Level | Header Syntax | Meaning & Desugaring |
| :--- | :--- | :--- |
| **1. Pure Default** | `In`<br>`gain: Gain`<br>`Frame[]` | Declares a `signal` entity with default properties (`default: 0.0`, inferred `type`, `rate`, `domain`). Empty brackets `[]` bind the array dimension to the incoming `Port.size`. |
| **2. Explicit Override** | `signal Out { rate: 100 }`<br>`gain: signal Gain { default: 1.0 }`<br>`reset: switch Reset { default: off }` | Whenever `{ ... }` properties or a non-`signal` type (`switch`) are specified, the explicit `<entityType>` (`signal` or `switch`) **must** precede the `<EntityName>`. |

> **Note:** Because `<propertyName>: <entityType> <EntityName> { ... }` (e.g., `gain: signal Gain { default: 1.0 }`) already matches Stride's core grammar where a property value can be an entity declaration, the header requires no special-case property grammar.

---

## 3. Body Properties: `code:` vs. `entities:` & `streams:`

Inside `{ ... }`, a callable's implementation can be expressed using either the unified `code:` property or the explicit `entities:` and `streams:` properties:

### Option A: Unified `code: [ ... ]` Property
Combines internal entity declarations and stream pipelines into a single list (mirroring the top-level scope of a `.stride` file):

```stride
module Input >> Accumulator(reset: switch Reset { default: off }) >> Output {
    code: [
        signal Total { default: 0.0  reset: Reset  persistent: on }
        Total + Input >> Total >> Output;
    ]
}
```

### Option B: Explicit `entities: [ ... ]` and `streams: [ ... ]` Properties
Separates internal declarations from stream pipelines explicitly:

```stride
module Input >> Accumulator(reset: switch Reset { default: off }) >> Output {
    entities: [
        signal Total { default: 0.0  reset: Reset  persistent: on }
    ]
    streams: [
        Total + Input >> Total >> Output;
    ]
}
```

---

## 4. Loop Modes & Declaration Properties

To avoid ambiguity between **external calling ports** and **internal loop mechanics**, `loop` uses `(...)` exclusively for external `propertyInputPort` declarations, and standard body properties (`iterator:`, `until:`, `maxIterations:`) inside `{ ... }` for loop control.

 *(Vectorized loops are intentionally omitted; elementwise array arithmetic is handled directly by stream vectorization.)*

### 4.1 Iterator Mode (`iterator:`) *(Deferred)*
> [!NOTE]
> Iterators are not currently supported in Stride and are deferred for future language versions. Loops currently operate in Short-Circuit Mode (`until:` / `maxIterations:`).

```stride
# Proposed future syntax:
# loop Frame[] >> FrameEnergy() >> signal Energy { default: 0.0 } {
#     iterator: signal Sample { type: Real }
#     code: [
#         Energy + (Sample * Sample) >> Energy;
#     ]
# }
```

### 4.2 Short-Circuit Mode (`until:` / `maxIterations:`)
Executes iteratively until the internal switch referenced by `until:` evaluates to `on` (or `maxIterations` is reached):

```stride
loop Guess >> SolveMakeup(target: signal Target { default: 1.0 }) >> BestGain {
    until: Converged
    maxIterations: 8
    code: [
        switch Converged { default: off }
        signal Next { default: 0.0 }

        Guess - ((Guess * Guess - Target) / (2.0 * Guess)) >> Next >> Guess >> BestGain;
        [Abs(Next - Guess), 0.0001] >> Less() >> Converged;
    ]
}
```

---

## 5. Bidirectional Attribute Resolution

Port entities (`signal` / `switch`) declared in a signature without explicit `type:`, `rate:`, or `domain:` overrides start with `Unresolved` attributes and resolve at call sites (`Producer >> Callable() >> Consumer`):

1. **Forward Propagation (Inputs $\to$ Outputs):**
   Concrete `type`, `rate`, and `domain` attributes on `Producer` bind to `MainInputs` and propagate forward to unconstrained `MainOutputs` and internal entities.
2. **Backward Propagation (Outputs $\to$ Inputs / Internal):**
   For source/generator modules without main inputs (e.g., `Phasor(freq: 440.0) >> AudioOut`) or when upstream inputs are untyped literals, unconstrained `MainOutputs` resolve their `domain`, `rate`, and `type` backward from `Consumer` (`AudioOut`), propagating inward to internal streams (allowing expressions like `Output.rate`).
3. **Explicit Overrides:**
   Any port that intentionally transforms `rate`, `type`, or `domain` declares that property explicitly in the header (e.g., `>> signal Out { rate: In.rate / 4 }`).

---

## 6. Desugaring & Validation Rules

### 6.1 Mutual-Exclusivity Validation
During schema/desugar validation, two strict mutual-exclusivity rules are enforced per callable:
1. **Header Ports vs. `ports:` Property:**
   If a callable declares ports in its header (`In >> Name(...) >> Out`), specifying `ports: [...]` inside `{ ... }` is a compile error.
2. **`code:` vs. `entities:` / `streams:` Properties:**
   If a callable specifies `code: [...]`, specifying `entities: [...]` or `streams: [...]` in the same callable is a compile error.

### 6.2 Canonical Desugaring Transformation
The desugaring pass lowers compact callables into the canonical AST structure before semantic analysis (`CodeAnalysis`, `CodeResolver`, `StrideGenerator`):

1. **Header Lowering:**
   * Each port in `<MainInputs>` generates a `mainInputPort` in `ports:` and appends its `signal`/`switch` declaration to `entities:`.
   * Each port in `(<PropertyPorts>)` generates a `propertyInputPort { name: "<propName>" entity: <EntityName> }` in `ports:` and appends its `signal`/`switch` declaration to `entities:`.
   * Each port in `<MainOutputs>` generates a `mainOutputPort` in `ports:` and appends its `signal`/`switch` declaration to `entities:`.
   * Port array dimensions using `[]` (e.g., `InBuffer[]`) desugar to `InBuffer[<SynthesizedPortName>.size]`.
2. **`code:` Partitioning:**
   * Every `AST::Declaration` inside `code: [...]` is appended to `entities: [...]`.
   * Every `AST::Stream` inside `code: [...]` is appended to `streams: [...]` in exact top-to-bottom textual order.

### 6.3 Side-by-Side Desugaring Example

#### Compact Source Syntax
```stride
module Input >> Amplifier(gain: signal Gain { default: 1.0 }) >> Output {
    code: [
        Input * Gain >> Output;
    ]
}
```

#### Canonical Desugared Equivalent
```stride
module Amplifier {
    ports: [
        mainInputPort InPort { entity: Input }
        mainOutputPort OutPort { entity: Output }
        propertyInputPort GainPort { name: "gain" entity: Gain }
    ]
    entities: [
        signal Input { default: 0.0 }
        signal Output { default: 0.0 }
        signal Gain { default: 1.0 }
    ]
    streams: [
        Input * Gain >> Output;
    ]
}
```

---

## 7. Complete Complex Example

Below is a full multi-rate, stateful **Spectral Sidechain Compressor** demonstrating how `module`, `buffer`, `loop` (both iterator and short-circuit modes), and `reaction` compose together using the compact header and `code:` syntax:

```stride
module [AudioIn[2], SidechainIn] 
    >> SpectralCompressor(
        threshold: signal Thresh  { default: -18.0 },
        ratio:     signal Ratio   { default: 4.0 },
        attack:    signal Attack  { default: 0.01 },
        release:   signal Release { default: 0.1 },
        reset:     switch Reset   { default: off }
    ) 
    >> [AudioOut[2], signal GainReduction { rate: 100 }] 
{
    code: [
        signal Env { default: 0.0  reset: Reset  persistent: on }
        switch OverThresh { default: off }

        buffer ScWindow {
            type:        Real
            capacity:    256
            frameSize:   256
            hop:         64
            mode:        Buffer::ModeSliding
            triggerMode: Buffer::TriggerPeriodic
            trigger:     Reset
        }

        # Short-Circuit Loop: iterative gain solver
        loop Guess >> SolveMakeup(target: signal Target { default: 1.0 }) >> BestGain {
            until: Converged
            maxIterations: 8
            code: [
                switch Converged { default: off }
                signal Next { default: 0.0 }
                signal Diff { default: 0.0 }

                Guess - ((Guess * Guess - Target) / (2.0 * Guess)) >> Next;
                Next - Guess >> Abs() >> Diff;
                Next >> Guess >> BestGain;
                [Diff, 0.0001] >> Less() >> Converged;
            ]
        }

        # Triggered Reaction: runs every 64 samples when ScWindow.ready fires
        reaction ComputeEnvelope(buffer: signal Win[] { type: Real }) >> DetectedLevel {
            code: [
                Win >> Sqrt() >> DetectedLevel;
            ]
        }

        # Processing Pipelines
        SidechainIn >> ScWindow;
        ScWindow.ready >> ComputeEnvelope(buffer: ScWindow) >> Env;

        [Env, Thresh] >> Greater() >> OverThresh;
        (Env - Thresh) * (1.0 - (1.0 / Ratio)) >> SolveMakeup(target: Ratio) >> GainReduction;

        AudioIn[0] * GainReduction >> AudioOut[0];
        AudioIn[1] * GainReduction >> AudioOut[1];
    ]
}
```