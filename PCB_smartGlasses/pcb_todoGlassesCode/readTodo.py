import pyautogui
import time 
import sys

def wait_for_input():
  """Waits for a terminal input.

  Returns:
    The input string.
  """

  while True:
    try:
      input = sys.stdin.readline().strip()
      if input:
        return input
      else:
         break
    except KeyboardInterrupt:
      print("Keyboard interrupt received.")
      sys.exit(0)
    time.sleep(0.1)

end = 0
update = 1
while not end:
    print("Please enter an input: update, stop, PRESS enter to read todo\n")
    input = wait_for_input()
    if input:
        print("Input received: {}".format(input))
        if input == "update":
            print("Move your mouse to the terminal\n")
            time.sleep(3)
            f = open("todo.txt", 'r')
            print("Values will now be printed\n")
            for x in f:
                for eachChar in x:
                    pyautogui.typewrite(str(eachChar) + '\n')
                time.sleep(2)
            update = 0
        elif input == "stop":
            end = 1
    else:
        if update:
            print("Move your mouse to the terminal\n")
            time.sleep(3)
            f = open("todo.txt", 'r')
            print("Values will now be printed\n")
            for x in f:
                for eachChar in x:
                    pyautogui.typewrite(str(eachChar) + '\n')
                time.sleep(2)
            update = 0
        else:
           continue