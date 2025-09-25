# 🏊 Object Pool Pattern - Documentación Técnica

## 📋 Tabla de Contenidos
- [Introducción](#introducción)
- [Arquitectura del Sistema](#arquitectura-del-sistema)
- [Decisiones de Diseño](#decisiones-de-diseño)
- [Flujo de Ejecución](#flujo-de-ejecución)
- [Casos de Uso](#casos-de-uso)
- [Resumen Técnico](#resumen-técnico)

## 🎯 Introducción

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

## 🏗️ Arquitectura del Sistema

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

## 🎯 Resumen Técnico

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