from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.chrome.options import Options
import time
import subprocess

print("COMPILING LOGIC SCRIPTS...")
compile_command = 'g++ -o find_cell find_cell.cpp'
subprocess.run(compile_command, shell=True)
print("COMPILATION COMPLETE.")

options = Options()
# options.add_argument('--headless')
options.page_load_strategy = 'eager'
driver = webdriver.Chrome(options=options)
driver.get('https://minesweeperonline.com/#beginner')
action = ActionChains(driver)

MAXR = 100
MAXC = 100
R = 9
C = 9
NUM_MINES = 10

# Click on the first cell
cell_1_1 = driver.find_element('xpath', '//*[@id="1_1"]')
cell_1_1.click()

board = [[-1 for _ in range(MAXC)] for _ in range(MAXR)]

while True:
    # get the board state
    for i in range(1, R+1):
        for j in range(1, C+1):
            cell = driver.find_element('xpath', f'//*[@id="{i}_{j}"]')
            class_text = cell.get_attribute('class')
            if 'open' in class_text:
                board[i][j] = int(class_text[-1])
            elif 'flag' in class_text:
                board[i][j] = -2
            else:
                board[i][j] = -1

    print("RUNNING LOGIC SCRIPT...")
    find_program = subprocess.Popen(['./find_cell'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # put in the board state
    find_program.stdin.write(f'{R} {C} {NUM_MINES}\n'.encode())
    for row in board[1:R+1]:
        for cell in row[1:C+1]:
            find_program.stdin.write(f'{cell} '.encode())
        find_program.stdin.write('\n'.encode())
    find_program.stdin.flush()

    # get the output
    output_id = find_program.stdout.readline().decode().strip()
    output = find_program.stdout.readline().decode().strip()
    print(output_id, output)

    cell = output.split()
    x = int(cell[0])
    y = int(cell[1])

    if output_id == '0':
        element = driver.find_element('xpath', f'//*[@id="{x}_{y}"]')
        action.context_click(element).perform()
    elif output_id == '1':
        driver.find_element('xpath', f'//*[@id="{x}_{y}"]').click()

    time.sleep(0.1)
    
    # check if the game is over
    face = driver.find_element('xpath', '//*[@id="face"]')
    face_class_text = face.get_attribute('class')
    if 'facewin' in face_class_text or 'facedead' in face_class_text:
        break

time.sleep(3)
driver.quit()