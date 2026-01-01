import sys

def limpiar_obj(input_file, output_file):
    print(f"Procesando {input_file}...")
    with open(input_file, 'r') as f_in, open(output_file, 'w') as f_out:
        f_out.write("# Optimizado para Simple Parser\n")
        f_out.write("o ObjetoLimpio\n")
        
        vertex_count = 0
        face_count = 0

        for line in f_in:
            parts = line.split()
            if not parts: continue
            
            tipo = parts[0]
            
            # 1. Mantener vértices geométricos (v x y z)
            if tipo == 'v':
                f_out.write(line)
                vertex_count += 1
                
            # 2. Limpiar caras: transformar "f v/vt/vn" a "f v"
            elif tipo == 'f':
                new_face = ["f"]
                for grupo in parts[1:]:
                    # Tomar solo el índice del vértice (antes del primer /)
                    indice_v = grupo.split('/')[0]
                    new_face.append(indice_v)
                
                f_out.write(" ".join(new_face) + "\n")
                face_count += 1

    print(f"¡Listo! Guardado en {output_file}")
    print(f"Vértices: {vertex_count}, Caras: {face_count}")

if __name__ == "__main__":
    # Uso: python optimizar_obj.py archivo_entrada.obj archivo_salida.obj
    input_name = "cyber-penger.obj" 
    output_name = "penger_limpio.obj"
    
    if len(sys.argv) > 1: input_name = sys.argv[1]
    if len(sys.argv) > 2: output_name = sys.argv[2]
    
    limpiar_obj(input_name, output_name)