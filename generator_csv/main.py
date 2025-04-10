import csv
import random
import string

# Определяем количество строк
num_rows = 100000000

# Открываем файл для записи
with open('large_file.csv', mode='w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file)
    # Записываем заголовки
    writer.writerow(['Column1', 'Column2', 'Column3'])
    # Записываем данные
    for _ in range(num_rows):
        writer.writerow([''.join(random.choices(string.ascii_letters + string.digits, k=10)),
                         ''.join(random.choices(string.ascii_letters + string.digits, k=10)),
                         ''.join(random.choices(string.ascii_letters + string.digits, k=10))])