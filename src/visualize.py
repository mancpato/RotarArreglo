import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import os

def cargar_datos(filepath, etiqueta_compilacion):
    if not os.path.exists(filepath):
        print(f"Advertencia: No se encontró {filepath}")
        return None
    try:
        df = pd.read_csv(filepath)
    except Exception as e:
        print(f"Error leyendo {filepath}: {e}")
        return None

    if 'Estado' in df.columns:
        df = df[df['Estado'] == 'OK'].copy()
    
    if 'Tiempo_ns' in df.columns:
        df['Tiempo_us'] = df['Tiempo_ns'] / 1000.0
    
    df['Compilacion'] = etiqueta_compilacion
    return df

def configurar_estilo():
    sns.set_theme(style="whitegrid")
    plt.rcParams.update({'figure.max_open_warning': 0})

def guardar_grafica(df, titulo, nombre_archivo, escala_log=False):
    if df.empty:
        return

    plt.figure(figsize=(12, 7))
    
    # Crear la gráfica
    ax = sns.lineplot(
        data=df, 
        x="Tamaño", 
        y="Tiempo_us", 
        hue="Algoritmo", 
        style="Compilación", 
        markers=True, 
        dashes=False,
        linewidth=2
    )

    # Configuración Logarítmica vs Lineal
    if escala_log:
        ax.set_yscale("log")
        plt.ylabel("Tiempo (microsegundos) - Escala Log", fontsize=12)
        titulo += " (Log Scale)"
    else:
        plt.ylabel("Tiempo (microsegundos)", fontsize=12)

    plt.title(titulo, fontsize=16)
    plt.xlabel("Tamaño del Arreglo (N)", fontsize=12)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', title="Algoritmo")
    
    plt.tight_layout()
    ruta_salida = f"figures/{nombre_archivo}"
    plt.savefig(ruta_salida, dpi=300)
    print(f"-> Generada: {ruta_salida}")
    plt.close()

if __name__ == "__main__":
    base_dir = "data/resultados"
    if not os.path.exists("figures"):
        os.makedirs("figures")
    
    configurar_estilo()

    # 1. Cargar Datos
    # Ajuste aquí si quiere ver debug/profile también
    archivos = [("test_release.csv", "Release")]
    
    dfs = []
    for archivo, etiqueta in archivos:
        df = cargar_datos(os.path.join(base_dir, archivo), etiqueta)
        if df is not None:
            dfs.append(df)
    
    if not dfs:
        print("No hay datos para graficar.")
        exit()

    df_total = pd.concat(dfs)

    # --- GRAFICA 1: GENERAL (Lineal) ---
    # Muestra la realidad: los lentos aplastan a los rápidos
    guardar_grafica(df_total, "Rendimiento General", "1_general_lineal.png")

    # --- GRAFICA 2: GENERAL (Logarítmica) ---
    # La solución científica: permite ver órdenes de magnitud diferentes
    guardar_grafica(df_total, "Rendimiento General (Log)", "2_general_log.png", escala_log=True)

    # --- GRAFICA 3: ZOOM A LOS RÁPIDOS (La "Fórmula 1") ---
    # Filtramos programáticamente para quitar los "lentos" (ej. > 50us promedio)
    # O excluimos explícitamente por nombre
    excluir = ['V1_Ingenuo', 'V2_Optimo', 'V9_Juggling', 'V11_Reversa', 'V6_Manual']
    
    # Filtramos: Que NO esté en la lista de excluir
    df_zoom = df_total[~df_total['Algoritmo'].isin(excluir)]
    
    if not df_zoom.empty:
        guardar_grafica(df_zoom, "Zoom: Algoritmos Optimizados (Memcpy/Static)", "3_zoom_rapidos.png")
    else:
        print("No quedaron algoritmos para el zoom.")