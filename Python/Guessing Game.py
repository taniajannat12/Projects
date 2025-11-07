import random

print("Welcome to the Number Guessing Game!")

r = random.randint(1, 10)

for attempt in range(1, 10):
    guess = int(input(f"Attempt {attempt}: Enter your guess: "))
    
    if guess < r:
        print("Too low😥 Try again😊.")
    elif guess > r:
        print("Too high😥 Try again😊.")
    else:
        print(f"Correct! You Are Won🎉 in {attempt} attempts")
        break
else:  # এই else for loop-এর সাথে align
    print(f"Sorry! You lost😥 The number was {r}")
