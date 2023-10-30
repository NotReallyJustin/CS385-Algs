x = "\""
for i in range(100000, 0, -1):
    x += f"{i} "

x += "\""

with open("other.txt", "w") as output:
    output.write(x)