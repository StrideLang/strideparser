# Stride Language Specification: Syntax & AST Terminology

This document defines the official syntactic constructs, grammar terminology, and AST concepts of the Stride language.

---

## 1. Syntactic Model Overview

Stride's top-level grammar is built upon two primary syntactic constructs:

1. **Entity Declarations:** Define structural building blocks, types, execution domains, data storage, callables, etc.
2. **Streams:** Expressions that connect data sources, operators, function calls, and destinations to express dataflow and processing pipelines.

**Properties** (`propertyName: value`) are the internal mechanism used inside both entity declarations (to specify attributes and configuration) and function calls / invocations (to specify named arguments and port connections).

### 1.1 Entity Syntactic Usage
Once declared, an **Entity** can be referenced syntactically in the following contexts:
* **Within Streams:**
  * By **Entity Name only** (e.g., as a stream source, sink, or signal variable: `AudioIn >> AudioOut`).
  * By **Entity Name with functional notation** (as an invocation / callable with properties: `AudioIn >> Gain(gain: 0.5) >> AudioOut`).
* **Within Properties & Lists:**
  * By **Entity Name only** when passed as a property value (e.g., `rate: AudioRate`) or as an element within a list (e.g., `[Voice1, Voice2]`).

> **Note:** Although the term "entity" in many languages implies an instance, in Stride an entity is a syntactical entity that can be referenced in other places in code. Instantation is defined in the *Stride Semantics Specification*.

> **Note on Semantics:** The syntactic layer defines structure and grammar only. Semantic interpretation (e.g., typing, reactive lifecycles, execution domains, and code generation) is decoupled from the syntax and defined in the *Stride Semantics Specification*.

---

## 2. Lexical Casing Rules & Reserved Keywords

### 2.1 Casing Rules
Stride enforces a strict syntactic distinction based on casing:

```
Entity Name   := [A-Z][A-Za-z0-9_]*    // Uppercase start
Entity Type   := [a-z][A-Za-z0-9_]*    // Lowercase start
Property Name := [a-z][A-Za-z0-9_]*    // Lowercase start
```

* **Entity Name (Uppercase):** Identifies declared entity instances (e.g., `Cutoff`, `Voices`, `MainDomain`, `Gain`).
* **Entity Type (Lowercase):** Specifies the category/type of the entity being declared (e.g., `signal`, `domain`, `module`, `reaction`, `type`).
* **Property Name (Lowercase):** Identifies key-value attributes in declarations and ports in function calls (e.g., `default`, `rate`, `in`, `out`, `gain`).

### 2.2 Built-in Literals & Keywords
Stride reserves lowercase keywords for control states and empty/null values:

* **State Literals (`on`, `off`):** Represent binary switch and boolean states.
  ```stride
  switch Enabled {
      default: off
  }
  ```
* **Empty / Null Literal (`none`):** Represents an unassigned value, empty port connection, or absent state.
  ```stride
  reset: none
  ```

---

## 3. Declarations

### 3.1 Entity Declaration
An **Entity Declaration** introduces a named entity into a scope with a lowercase entity type, an uppercase entity name, and an optional body containing properties. Any nested entity declarations are contained within property values.

```stride
<entityType> <EntityName> {
    <propertyName>: <propertyValue>
}
```

*Example:*
```stride
type Name {
    default: 1000.0
    property: Value
}
```

### 3.2 Array Declaration
An **Array Declaration** introduces an indexed collection of entities with an explicit dimension. Syntactically, the size expression within brackets can be an integer literal or an identifier/expression (such as `Port.size`) resolved during compilation.

```stride
<entityType> <EntityName>[<SizeExpression>] {
    <properties...>
}
```

*Examples:*
```stride
signal ArrayName[4] {
    default: 0.0
}

signal Channels[InPort.size] {
    default: 0.0
}
```

### 3.3 Declaration Properties
Inside an entity declaration body, **Properties** configure attributes, initial states, or nested structures for that entity.

```stride
default: 440.0
rate: DomainRate
```

---

## 4. Streams & Invocations

### 4.1 Streams
A **Stream** is a syntactic expression that routes data between sources, transformations, and destinations using stream operators (e.g., `>>`).

```stride
AudioIn >> LowPass(cutoff: 1000.0) >> AudioOut
```

### 4.2 Invocations (Function Calls)
An **Invocation** applies a callable entity by its `Entity Name` using functional notation, taking a set of properties that bind arguments to its ports.

```stride
Process(parameter: 0.75)
```

### 4.3 Invocation Properties
Inside an invocation, **Properties** map expressions, variables, or literals to the callee's named ports.

```stride
in: InputSignal
gain: 0.75
```

---

## 5. Member & Index Access

* **Member Access (`Target.member`):** Uses dot notation to reference child declarations (by `Entity Name`) or internal properties/ports (by `Property Name`).
  ```stride
  OscillatorSignal.out      // Accessing property 'out'
  AudioDomain.rate    // Accessing property 'rate'
  ```
* **Index Access (`Target[index]`):** Uses bracket notation to address an individual element within an array entity.
  ```stride
  Voices[0]
  ```

---

## 6. Syntactic Terminology Reference

| Syntactic Term | Lexical Form / Grammar | Context | Example |
| :--- | :--- | :--- | :--- |
| **Entity Name** | `[A-Z][A-Za-z0-9_]*` | Entity instance names | `Cutoff`, `Voices`, `Gain` |
| **Entity Type** | `[a-z][A-Za-z0-9_]*` | Entity type / declaration keyword | `signal`, `domain`, `module` |
| **Property Name** | `[a-z][A-Za-z0-9_]*` | Keys for properties and ports | `default`, `rate`, `in`, `out` |
| **Literals / Keywords** | `on`, `off`, `none` | Reserved literals for state/null | `default: off`, `reset: none` |
| **Entity Declaration** | `entityType EntityName { ... }` | Single entity declaration | `signal Cutoff { ... }` |
| **Array Declaration** | `entityType EntityName[Size] { ... }` | Array entity declaration | `signal Voices[4] { ... }` |
| **Stream** | `Source >> Transform >> Sink` | Dataflow pipeline expression | `In >> Filter() >> Out` |
| **Invocation** | `CalleeName( ... )` | Function/module call expression | `Gain(in: AudioIn, out: Out)` |
| **Property** | `propertyName: Value` | Inside declarations or invocations | `rate: 44100`, `in: AudioIn` |
| **Member Access** | `Target.member` | Dot-notation accessor | `Filter.out` |
| **Index Access** | `Target[index]` | Bracket-notation accessor | `Voices[0]` |
