for i in range(-3, 10, 2):
    for j in range(-3, 10, 2):
        print("[", i, j, (((i+j) % 3) - 1) * 4, "]", end=' ')
    print()