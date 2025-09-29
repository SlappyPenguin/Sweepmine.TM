import subprocess
import time

R, C, N, T, O, num_cases = map(int, input().split())

# compile_command = subprocess.Popen(['g++', 'grader.cpp', 'sweepmine.cpp', '-o', 'sweepmine'], stdout=subprocess.PIPE)

count_wins = 0

for _ in range(num_cases):
    grader_program = subprocess.Popen(['./sweepmine'], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    grader_program.stdin.write(f"{R} {C} {N} {T} {O} 0\n".encode())
    grader_program.stdin.flush()

    answer = grader_program.stdout.readline().decode().strip()
    print("Game", str(_+1)+":", answer)
    if "win" in answer:
        count_wins += 1

print(str(count_wins / num_cases * 100) + " % win rate")