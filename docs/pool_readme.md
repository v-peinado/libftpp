# 🏊 Object Pool Pattern - Documentación Técnica

## 📋 Tabla de Contenidos
- [Introducción](#introducción)
- [Arquitectura del Sistema](#arquitectura-del-sistema)
- [Decisiones de Diseño](#decisiones-de-diseño)
- [Flujo de Ejecución](#flujo-de-ejecución)
- [Casos de Uso](#casos-de-uso)
- [Resumen Técnico](#resumen-técnico)

---

## Introducción

El **Object Pool** es un patrón de diseño que pre-alloca objetos para reutilizarlos, evitando el overhead de crear/destruir memoria constantemente. Es crítico en sistemas de alto rendimiento como game engines, servidores y trading systems.

### El Problema que Resuelve
```cpp
// ❌ PROBLEMA: Allocaciones constantes
for(int frame = 0; frame < 1000000; frame++) {
    Bullet* bullet = new Bullet();    // Malloc costoso
    bullet->fire();
    delete bullet;                    // Free costoso
}

// ✅ SOLUCIÓN: Pool con objetos reutilizables
Pool<Bullet> bulletPool(100);
for(int frame = 0; frame < 1000000; frame++) {
    auto bullet = bulletPool.acquire();  // O(1), sin malloc
    bullet->fire();
}  // Return automático, sin delete
```

---

## Arquitectura del Sistema

### Estructura de Datos Core
```cpp
template<typename TType>
class Pool {
    std::vector<TType*> m_objects;    // Punteros a objetos pre-allocados
    std::vector<size_t> m_available;  // Indices disponibles
};
```

### Variables Principales:
- **`m_objects`**: Vector de punteros a todos los objetos allocados. Tamaño fijo después de `resize()`.
- **`m_available`**: Vector que contiene índices de objetos no utilizados.

### Wrapper RAII
```cpp
class Object {
    TType* m_ptr;           // Puntero al objeto
    Pool<TType>* m_pool;    // Referencia al pool padre
    size_t m_index;         // Índice en m_objects
    
    ~Object() {
        if (m_ptr && m_pool) {
            m_pool->returnToPool(m_index);
        }
    }
};
```
---

## 💡 Decisiones de Diseño

### El Diseño Elegido: Dos Vectores con Índices
```cpp
m_objects:   [Obj0][Obj1][Obj2][Obj3][Obj4]  // Posiciones fijas
m_available: [0, 2, 4]                       // Índices disponibles (stack)
// Implica: 0✅ 1❌ 2✅ 3❌ 4✅ (flags implícitos)
```

### ¿Por Qué Índices y No Punteros?

**Razón #1: Return en O(1)**
```cpp
class Object {
    size_t m_index;  // Guardamos el índice
    
    ~Object() {
        m_pool->returnToPool(m_index);  // Solo push_back - O(1)
    }
}
```

**Razón #2: Memoria Estable**
- Los objetos NUNCA se mueven en memoria
- Los punteros en `m_objects` son permanentes

**Razón #3: Debugging y Trazabilidad**
```cpp
// Con índices puedes identificar:
"Object #3 está en uso"
"Pool tiene objetos 0,2,4 disponibles"
```

### Análisis de Alternativas

#### Alternativa 1: Dos Vectores de Punteros ❌
```cpp
std::vector<TType*> m_available_objects;
std::vector<TType*> m_in_use_objects;
```
**Problemas:**
- Return requiere búsqueda O(n) en `m_in_use`
- Mover punteros entre vectores constantemente
- Mayor uso de memoria (punteros duplicados)

#### Alternativa 2: Vector + Flags Booleanos ❌
```cpp
std::vector<TType*> m_objects;
std::vector<bool> m_is_available;  // [true,false,true,false,true]
```
**Problemas:**
- Acquire requiere buscar O(n) el primer `true`
- No hay cola de disponibles pre-computada
- `std::vector<bool>` tiene problemas conocidos, es considerado un error de diseño en la STL

#### Nuestra Solución: Índices como Identificadores ✅
```cpp
std::vector<TType*> m_objects;     // Array fijo
std::vector<size_t> m_available;   
```
**Ventajas:**
- ✅ Todas las operaciones en O(1) real
- ✅ Mínimo uso de memoria
- ✅ Todos los objetos son contiguos

### ¿Por Qué Vector y No std::stack?

```cpp
// std::stack es solo un wrapper innecesario
std::stack<size_t> stack;  // Internamente usa deque (memoria fragmentada)

// Vector usado como stack es óptimo
std::vector<size_t> m_available;  // Memoria contigua, sin overhead
```

**Usamos vector directamente porque:**
- Memoria contigua
- Mayor flexibilidad (podemos iterar para debug si es necesario)
- Menos capas de abstracción = menos overhead y complejidad

### Por Qué Esta Arquitectura

La decisión de usar **dos vectores con índices** combina tres conceptos:

1. **Sistema de Flags Implícito**: La presencia/ausencia en `m_available` actúa como flag
2. **Ready Queue**: `m_available` es una cola O(1) de objetos listos
3. **Identificador Único**: El índice es una ID permanente del objeto

---

## 🔄 Flujo de Ejecución

### 1️⃣ **Construcción**
```cpp
Pool<TestObject> myPool;
```
- Estado inicial: vectores vacíos
- `m_objects = []`
- `m_available = []`

### 2️⃣ **Pre-allocación con resize()**
```cpp
myPool.resize(5);
```
Operaciones:
1. Alloca 5 objetos con `new TType()`
2. Almacena punteros en `m_objects[0..4]`
3. Puebla `m_available` con [0,1,2,3,4]

Estado resultante:
- `m_objects = [ptr0, ptr1, ptr2, ptr3, ptr4]`
- `m_available = [0, 1, 2, 3, 4]`

### 3️⃣ **Adquisición con acquire()**
```cpp
Pool<TestObject>::Object obj1 = myPool.acquire(params);
```
Secuencia de operaciones:
1. `index = m_available.back()` → Obtiene último índice
2. `m_available.pop_back()` → Remueve de disponibles
3. `ptr = m_objects[index]` → Obtiene puntero
4. `ptr->~TType()` → Llama destructor explícitamente
5. `new (ptr) TType(params)` → Placement new con parámetros
6. Retorna `Object(ptr, this, index)` → Wrapper RAII

### 4️⃣ **Devolución con returnToPool()**
```cpp
void returnToPool(size_t index) {
    m_available.push_back(index);
}
```
- Solo añade índice al stack
- No llama destructor del objeto
- No libera memoria
- Objeto queda disponible para reutilización

### 5️⃣ **Ejemplo de Flujo Completo**

```cpp
// INICIALIZACIÓN: Pool con 5 objetos pre-allocados
Pool<TestObject> pool;
pool.resize(5);
// Estado interno:
// m_objects = [obj0*, obj1*, obj2*, obj3*, obj4*]
// m_available = [0, 1, 2, 3, 4]

// ACQUIRE: Obtiene objeto en índice 4
auto obj1 = pool.acquire();
// Estado interno:
// m_available = [0, 1, 2, 3]  (índice 4 removido del stack)
// El wrapper Object guarda: m_ptr=obj4*, m_index=4

// USO: Acceso directo al objeto mediante operator->
obj1->doWork();

// RETURN AUTOMÁTICO: Scope anidado demuestra RAII
{
    auto obj2 = pool.acquire();  // Obtiene índice 3
    // m_available = [0, 1, 2]
    obj2->doWork();
}  // Destructor de obj2 ejecuta returnToPool(3)
// m_available = [0, 1, 2, 3]  (índice 3 retornado - LIFO)

// REUTILIZACIÓN: El mismo objeto físico, reinicializado
auto obj3 = pool.acquire();  // Obtiene índice 3 nuevamente
// El objeto en m_objects[3] es reconstruido con placement new
```

### 6️⃣ **Ciclo de Vida del Objeto**

```cpp
// Estado inicial
m_objects:   [obj0, obj1, obj2]
m_available: [0, 1, 2]

// acquire() - índice 2
m_objects:   [obj0, obj1, obj2]  // Sin cambios
m_available: [0, 1]               // 2 removido

// acquire() - índice 1
m_objects:   [obj0, obj1, obj2]  // Sin cambios
m_available: [0]                  // 1 removido

// ~Object() - return índice 2
m_objects:   [obj0, obj1, obj2]  // Sin cambios
m_available: [0, 2]               // 2 añadido (LIFO)

// acquire() - reutiliza índice 2
m_objects:   [obj0, obj1, obj2]  // obj2 reconstruido in-place
m_available: [0]                  // 2 removido nuevamente
```

### Detalles de Implementación

#### Stack LIFO con Vector
```cpp
// m_available funciona como stack:
acquire():  m_available.back() + pop_back()   // O(1)
return():   m_available.push_back()           // O(1)
```
- Los índices quedan desordenados tras uso (LIFO)
- El desorden es irrelevante - cualquier índice disponible es válido

#### Placement New
```cpp
// En acquire():
ptr->~TType();                                   // Destructor explícito
new (ptr) TType(std::forward<TArgs>(args)...);  // Constructor in-place
```
- Reutiliza memoria existente
- Evita allocación/deallocación
- Permite pasar parámetros al constructor

#### Características RAII
```cpp
class Object {
    // Move-only semantics
    Object(Object&& other) noexcept;      // ✅ Movible
    Object(const Object&) = delete;       // ❌ No copiable
    
    ~Object() {
        if (m_ptr && m_pool) {
            m_pool->returnToPool(m_index); // Garantizado
        }
    }
}
```

#### Exception Safety
```cpp
template<typename... TArgs>
Object acquire(TArgs&&... args) {
    size_t index = m_available.back();
    m_available.pop_back();
    
    try {
        new (m_ptr) TType(std::forward<TArgs>(args)...);
    }
    catch(...) {
        m_available.push_back(index);  // Rollback seguro
        throw;
    }
}
```

---

## 📱 Casos de Uso

### Game Development
```cpp
// Bullets, particles, enemies
Pool<Bullet> bulletPool(1000);
Pool<Particle> particlePool(10000);
Pool<Enemy> enemyPool(50);
```

### Network Servers
```cpp
// Connection handlers
Pool<Connection> connectionPool(maxClients);
Pool<Request> requestPool(maxRequests);
```

---

## Resumen Técnico

El Pool implementa un patrón de reutilización de objetos mediante:

1. **Pre-allocación**: Todos los objetos se crean al inicio
2. **Índices como identificadores**: Permiten return en O(1)
3. **RAII con Object wrapper**: Garantiza devolución automática
4. **Placement new**: Reinicializa objetos sin reallocation

### Puntos Clave del Diseño:
- **m_objects**: Vector a modo de Array fijo de punteros, nunca cambia después de resize()
- **m_available**: Vector a modo de LIFO que mantiene índices libres
- **Object wrapper**: Gestiona ciclo de vida mediante RAII
- **Reutilización**: Los objetos se reconstruyen in-place, sin nueva allocación

### Complejidad Algorítmica:

| Operación | Complejidad | Descripción |
|-----------|-------------|-------------|
| `resize()` | O(n) | n allocaciones iniciales |
| `acquire()` | O(1) | pop_back + placement new |
| `returnToPool()` | O(1) | push_back |
| `Object::operator->()` | O(1) | acceso directo a puntero |

El diseño prioriza:
- Cero allocaciones durante uso normal
- Todas las operaciones en O(1) (excepto inicialización)
- Exception safety mediante RAII
- Move semantics para el wrapper Object

---

## Conceptos Importantes

### **RAII (Resource Acquisition Is Initialization)**

**Concepto Fundamental**: Patrón donde la adquisición de un recurso está ligada a la inicialización de un objeto, y la liberación del recurso está ligada a la destrucción del objeto. El ciclo de vida del objeto controla automáticamente el ciclo de vida del recurso.

**Principio Core**: El recurso se adquiere en el constructor y se libera en el destructor. Como C++ garantiza que los destructores se llaman automáticamente (incluso con excepciones), los recursos siempre se liberan correctamente.

**Garantías que Proporciona**:
- **Exception Safety**: Los destructores se ejecutan durante stack unwinding
- **Determinístico**: Se sabe exactamente cuándo se libera el recurso
- **Composable**: Los objetos RAII pueden contener otros objetos RAII
- **No-leak guarantee**: Imposible tener leaks si se sigue el patrón

**En el Pool**: El `Object` wrapper es RAII puro - adquiere un objeto del pool en su construcción y lo devuelve en su destrucción. El destructor siempre se ejecuta, garantizando la devolución.

**Filosofía**: RAII convierte problemas de gestión de recursos en problemas de gestión de objetos. Como C++ gestiona objetos automáticamente, obtienes gestión de recursos automática.

### **Strong Exception Guarantee con Rollback Semantics**

**Concepto**: Garantía de que si una operación falla, el estado del sistema queda exactamente como estaba antes del intento.

**El Patrón**:
```
1. Modificar estado
2. try { operación peligrosa }
3. catch { 
    devolvemos al estado inicial
    throw;  // re-lanzar excepción
}
4. Retorno (solo si no hubo excepción)
```

**En el Pool**: El `acquire()` implementa Strong Guarantee:
- Saca el índice del stack
- Intenta construir el objeto
- Si falla: devuelve el índice y re-lanza
- Si éxito: crea el wrapper Object

**Importancia del `throw;`**: Re-lanzar la excepción es crucial. Sin él, la función continuaría y retornaría un Object con un puntero a memoria mal construida. El `throw;` interrumpe el flujo, previniendo la creación del wrapper RAII cuando el recurso no es válido.

**Principio Fundamental**: Este patrón separa dos responsabilidades:
1. Mantener consistencia interna (rollback del estado)
2. Informar del error al llamador (re-throw)

El sistema se auto-repara pero no oculta errores. Es transparente sobre fallos mientras mantiene integridad.

---

### **Rule of Five**

**Concepto Fundamental**: Si una clase necesita definir cualquiera de los cinco miembros especiales (destructor, constructor de copia, operador de asignación, constructor de movimiento, operador de asignación por movimiento), probablemente necesita definir los cinco.

**Principio Core**: Si tu clase gestiona recursos directamente (memoria raw, handles, etc.), necesitas controlar todas las formas en que ese recurso puede ser copiado, movido o destruido. Omitir alguno llevará a bugs sutiles.

**Los Cinco Miembros**:
1. **Destructor**: Libera recursos
2. **Constructor de copia**: Duplica recursos
3. **Operador de asignación**: Libera antiguos, duplica nuevos
4. **Constructor de movimiento**: Transfiere ownership
5. **Operador de asignación por movimiento**: Libera antiguos, transfiere nuevos

**En el Pool**: El `Object` wrapper implementa una variante - define destructor y movimiento pero **elimina** copia (porque no tiene sentido tener dos wrappers para el mismo objeto del pool). Es "Rule of Five consciente" - considera los cinco pero solo implementa los que tienen sentido semánticamente.

**Señal de Alerta**: Si escribes un destructor custom, PARA y piensa en los otros cuatro. El compilador generará versiones por defecto que probablemente están mal para tu caso.

---

### **Rule of Zero**

**Concepto Fundamental**: Las clases deberían gestionar cero recursos directamente. Usa tipos que ya siguen RAII (smart pointers, containers STL) y deja que el compilador genere los cinco miembros especiales automáticamente.

**Principio Core**: Delega la gestión de recursos a clases dedicadas. Tu clase de lógica de negocio no debería preocuparse por gestión de memoria - eso es trabajo de `unique_ptr`, `vector`, etc.

**La Regla**: Si puedes evitar escribir cualquiera de los cinco miembros especiales, evítalos todos. Los defaults del compilador serán correctos si todos tus miembros son RAII-compliant.

**Beneficios**:
- Menos código que mantener
- Menos bugs (los defaults son correctos)
- Automáticamente exception-safe
- Automáticamente move-enabled

**En el Pool**: La clase `Pool` principal sigue Rule of Zero - usa `vector` para gestionar memoria, no define ningún member especial. Los vectors se encargan de todo. Solo el `Object` wrapper necesita Rule of Five porque gestiona el "recurso" abstracto de "objeto prestado del pool".

**Filosofía**: Es una jerarquía - las clases de bajo nivel (como `unique_ptr`) siguen Rule of Five, las clases de alto nivel siguen Rule of Zero usando las de bajo nivel. No todas las clases deberían gestionar recursos - la mayoría deberían solo usar gestores existentes.

---

### **Separación Declaración-Implementación en Templates**

**Concepto Fundamental**: Cuando separas la implementación de la declaración en templates (especialmente con clases anidadas), la sintaxis se vuelve verbosa y compleja debido a que debes especificar completamente el scope y todos los parámetros de template.

**Desglose de la Sintaxis Más Compleja**:

```cpp
template<typename TType>                    // 1. Template de la clase contenedora
template<typename... TArgs>                 // 2. Template del método
typename Pool<TType>::Object                // 3. Tipo de retorno (necesita typename)
Pool<TType>::                              // 4. Scope de la clase
acquire(TArgs&&... p_args)                 // 5. Nombre del método y parámetros
```

**Por qué cada parte:**

1. **`template<typename TType>`** - Le dice al compilador "esto es parte de una clase template con parámetro TType"

2. **`template<typename... TArgs>`** - El método tiene sus propios templates (variadic en este caso)

3. **`typename Pool<TType>::Object`** - El retorno es un tipo anidado:
   - `typename` porque `Object` depende de `TType` 
   - Sin `typename`, el compilador no sabe si `Object` es tipo o variable
   - `Pool<TType>::` especifica de dónde viene `Object`

4. **`Pool<TType>::`** - Scope completo indicando que `acquire` pertenece a `Pool<TType>`

5. **`acquire(TArgs&&... p_args)`** - Finalmente el método con forwarding references

**Evolución de Complejidad**:

```cpp
// NIVEL 1: Método simple
// Dentro:  void clear();
// Fuera:   
template<typename T>
void Pool<T>::clear() { }

// NIVEL 2: Retorna tipo anidado
// Dentro:  Object get();
// Fuera:   
template<typename T>
typename Pool<T>::Object    // typename necesario
Pool<T>::get() { }

// NIVEL 3: Método de clase anidada
// Dentro de Object:  void reset();
// Fuera:
template<typename T>
void Pool<T>::Object::       // Doble scope resolution
reset() { }

// NIVEL 4: Constructor de clase anidada con inicializadores
// Dentro:  Object(TType* ptr);
// Fuera:
template<typename T>
Pool<T>::Object::            // No typename en constructores
Object(T* ptr) : m_ptr(ptr) { }

// NIVEL 5: La bestia - template de template con tipo dependiente
template<typename TType>
template<typename... TArgs>
typename Pool<TType>::Object 
Pool<TType>::acquire(TArgs&&... p_args) { }
```

**Reglas Clave**:

1. **`typename`** necesario cuando:
   - El tipo depende de un template parameter
   - Está dentro de un scope dependiente (`Pool<T>::`)
   - NO en constructores/destructores

2. **Orden de templates**:
   - Primero el de la clase
   - Luego el del método
   - No puedes combinarlos en uno

3. **Scope resolution (`::`)**: 
   - Debes especificar la ruta completa
   - `Pool<T>::Object::metodo` para métodos de clase anidada

**Por Qué Esta Complejidad Existe**:

- Los templates se compilan cuando se usan, no cuando se definen, es decir, si se usase la misma template con dos tipos diferentes se compilarian 2 funciones, si se usasen 0 tipos se compilarian 0 funciones.
- El compilador necesita distinguir entre tipos y valores
- Existe una dependencia de tipos `Object` solo si `Pool<T>` se instancia con un `T` específico


