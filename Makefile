# --- Makefile Multi-Target para Benchmark de Rotación ---

# Compilador
CC = gcc

# Directorios
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
DATA_DIR = data/resultados

# Nombres de los ejecutables
EXE_RELEASE = rotar_release
EXE_DEBUG   = rotar_debug
EXE_PROFILE = rotar_profile

# Archivos fuente y objetos
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Banderas base (Comunes)
# Quitamos -march=native para evitar segfaults en Release por ahora
CFLAGS_BASE = -I$(INC_DIR) -Wall -Wextra -std=c11
LDFLAGS = -lm

# --- Objetivos (Targets) ---

.PHONY: all release debug profile clean help run run-profile

# Por defecto, compila release
all: release

# 1. RELEASE (Optimizado) -> bin/rotar_release
# Forzamos 'clean' antes para asegurar que los .o se recompilen con las flags correctas
release: CFLAGS = $(CFLAGS_BASE) -O3 -DNDEBUG
release: clean
release: $(BIN_DIR)/$(EXE_RELEASE)
	@echo "--- ✅ Compilado RELEASE: $(BIN_DIR)/$(EXE_RELEASE) ---"

# 2. DEBUG (Símbolos) -> bin/rotar_debug
debug: CFLAGS = $(CFLAGS_BASE) -g -O0 -DDEBUG
debug: clean
debug: $(BIN_DIR)/$(EXE_DEBUG)
	@echo "--- 🐞 Compilado DEBUG: $(BIN_DIR)/$(EXE_DEBUG) ---"

# 3. PROFILE (Gprof) -> bin/rotar_profile
profile: CFLAGS = $(CFLAGS_BASE) -pg -O2
profile: clean
profile: $(BIN_DIR)/$(EXE_PROFILE)
	@echo "--- 📊 Compilado PROFILE: $(BIN_DIR)/$(EXE_PROFILE) ---"

# 4. STATIC (Análisis estático)
static:
	@echo "--- 🔍 Ejecutando Cppcheck ---"
	cppcheck --enable=all --inconclusive --std=c11 -I$(INC_DIR) $(SRC_DIR)

# --- Reglas de Construcción Genericas ---

# Regla para linkear cualquiera de los ejecutables
# El '%' captura el nombre (rotar_release, rotar_debug, etc.)
$(BIN_DIR)/%: $(OBJS) | $(BIN_DIR) $(DATA_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CFLAGS)

# Regla para compilar objetos (.c -> .o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Crear directorios
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

# --- Comandos de Ejecución ---

# Ejecutar Release
run: release
	@echo "--- Ejecutando Benchmark (Release) ---"
	./$(BIN_DIR)/$(EXE_RELEASE)

# Ejecutar Profile
run-profile: profile
	@echo "--- Ejecutando Profiler ---"
	./$(BIN_DIR)/$(EXE_PROFILE)

# Limpiar todo
clean:
	@echo "--- Limpiando temporales ---"
	rm -rf $(BUILD_DIR) gmon.out
	# Nota: No borramos 'bin' completo para conservar los ejecutables anteriores si se quiere,
	# pero como recompilamos los objetos, es mejor limpiar el binario especifico en la regla.
	# Para simplificar, aquí borramos los objetos para forzar recompilación.

full-clean: clean
	rm -rf $(BIN_DIR)

help:
	@echo "Opciones disponibles:"
	@echo "  make release     : Crea bin/rotar_release (Optimizado)"
	@echo "  make debug       : Crea bin/rotar_debug (Para GDB)"
	@echo "  make profile     : Crea bin/rotar_profile (Para Gprof)"
	@echo "  make run         : Compila y ejecuta la versión Release"