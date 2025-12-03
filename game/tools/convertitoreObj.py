import sys

def parse_obj_to_c_array(file_path):
    vertices = []
    normals = []
    tex_coords = []
    
    # Buffer finale per le stringhe
    output_lines = []

    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Errore: File '{file_path}' non trovato.")
        return

    # 1. Parsing dei dati grezzi
    for line in lines:
        parts = line.strip().split()
        if not parts:
            continue

        if parts[0] == 'v':
            vertices.append([float(x) for x in parts[1:4]])
        elif parts[0] == 'vn':
            normals.append([float(x) for x in parts[1:4]])
        elif parts[0] == 'vt':
            tex_coords.append([float(x) for x in parts[1:3]])
        elif parts[0] == 'f':
            # Gestione delle facce (triangolazione semplice)
            face_data = parts[1:]
            
            # Se è un quad (4 vertici), lo dividiamo in due triangoli: (0, 1, 2) e (0, 2, 3)
            triangles = []
            if len(face_data) == 3:
                triangles.append(face_data)
            elif len(face_data) == 4:
                triangles.append([face_data[0], face_data[1], face_data[2]])
                triangles.append([face_data[0], face_data[2], face_data[3]])
            
            for triangle in triangles:
                for vertex_str in triangle:
                    # Formato obj: v/vt/vn oppure v//vn oppure v/vt
                    vals = vertex_str.split('/')
                    
                    # Indici (OBJ parte da 1, Python da 0)
                    v_idx = int(vals[0]) - 1
                    
                    # Texture Coords (gestione se mancanti)
                    vt_idx = int(vals[1]) - 1 if len(vals) > 1 and vals[1] else -1
                    
                    # Normals (gestione se mancanti)
                    vn_idx = int(vals[2]) - 1 if len(vals) > 2 and vals[2] else -1

                    # Recupero dati
                    px, py, pz = vertices[v_idx]
                    
                    # Default Texture a 0.0 se mancanti
                    tu, tv = (0.0, 0.0)
                    if vt_idx >= 0:
                        tu, tv = tex_coords[vt_idx]
                    
                    # Default Normal a 0.0 se mancanti
                    nx, ny, nz = (0.0, 0.0, 0.0)
                    if vn_idx >= 0:
                        nx, ny, nz = normals[vn_idx]

                    # Formattazione stringa (aggiunge 'f' alla fine)
                    line_str = (f"{px:.4f}f, {py:.4f}f, {pz:.4f}f,  "
                                f"{nx:.4f}f, {ny:.4f}f, {nz:.4f}f,  "
                                f"{tu:.4f}f, {tv:.4f}f,")
                    output_lines.append(line_str)

    # 2. Stampa output formattato
    with open("output.txt", "w") as f:
           
        f.write("// positions          // normals           // texture coords\n")
        for line in output_lines:
            f.writelines(f"    {line}\n")

# --- ESEMPIO DI UTILIZZO ---
# Salva il tuo modello come 'model.obj' nella stessa cartella o cambia il percorso qui sotto.
if __name__ == "__main__":
    # Sostituisci con il percorso del tuo file obj
    input_file = "skybox.obj" 
    
    # Crea un file dummy per testare se non ne hai uno
    import os
    if not os.path.exists(input_file):
        with open(input_file, "w") as f:
            f.write("""
v -0.5 -0.5 -0.5
v 0.5 -0.5 -0.5
v 0.5 0.5 -0.5
v -0.5 0.5 -0.5
vt 0.0 0.0
vt 1.0 0.0
vt 1.0 1.0
vt 0.0 1.0
vn 0.0 0.0 -1.0
f 1/1/1 2/2/1 3/3/1
f 3/3/1 4/4/1 1/1/1
            """)
        print(f"Creato file di test: {input_file}")

    parse_obj_to_c_array(input_file)