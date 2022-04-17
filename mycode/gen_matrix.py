import numpy as np

screen_width = 16 * 75
screen_height = 9 * 75
tile_size = 25

mat = np.ones(shape=(screen_height//tile_size, screen_width//tile_size)).astype(int)

for i in range(mat.shape[0]):
    print("{", end='')
    print(mat[i][0], end ='')
    for num in mat[i][1:]:
        print(f", {num}", end='')
    if i < mat.shape[0]-1:
        print("},", end='\n')
    else:
        print("}", end='')