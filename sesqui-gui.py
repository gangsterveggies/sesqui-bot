#/usr/bin/python3

from tkinter import *
import random
import sys
import os

def domove(state, bot='mcts'):
    cmd = './SESQUIBOT -i ' + '"' + state  +'" --' + bot
    out = os.popen(cmd).read()
    return out

# use a class definition to hold all my functions
class SesquiGUI(Tk):
    def __init__(self):
        """ initialize the frame and widgets"""
        Tk.__init__(self)

        fr = Frame(self)

        #intialize operation buttons and the label
        self.second = None
        self.player_button = Button(self, text="Me",
                                    command=lambda: self.do_start(TRUE),
                                    font=("Helvetica", 16))
        self.computer_button = Button(self, text="CPU",
                                      command=lambda: self.do_start(FALSE),
                                      font=("Helvetica", 16))
        self.exit_button = Button(self, text="Exit", command=self.do_exit,
                                  font=("Helvetica", 10))
        self.again_button = Button(self, text="again?", command=self.do_again,
                                   state=NORMAL,
                                   font=("Helvetica", 10))
        self.myLabel = Label(self, text="Who starts?", font=("Helvetica", 16))

        self.state = ""
        self.bot = StringVar(self)
        self.bot.set("mcts") # initial value
        self.option_menu  = OptionMenu(self, self.bot, "ab", "mcts", "minimax", "random")


        self.buttons = [[] for i in range(8)]
        for i in range(8):
            for j in range(8):
                self.buttons[i].append(Button(self, text="---",
                                 command=lambda j=i, k=j: self.do_button(j, k),
                                 state=DISABLED, relief=RAISED, height=2,
                                   width=5, font=("Helvetica", 16)))

        #grid everything into the frame
        fr.grid()
        self.myLabel.grid(row=0, columnspan=4)
        self.player_button.grid(row=1, column=0)
        self.option_menu.grid(row=1, column=2)
        self.computer_button.grid(row=1, column=3)
        self.exit_button.grid(row=(5+8), column=0)
        self.again_button.grid(row=(5+8), column=2)
        for i in range(8):
            for j in range(8):
                self.buttons[i][j].grid(row=(i+3), column=j)
                
            

    def do_start(self,player):
        """start the game, if player is TRUE, player make first move"""
        #turn Me, computer, and again? buttons off
        self.player_button.config(state=DISABLED)
        self.computer_button.config(state=DISABLED)
        #self.again_button.config(state=DISABLED)

        #reset the bState and mySums lists and the flags
        self.bState = [[0, 0, 0, 0, 0, 0, 0, 0] for i in range(8)]
        self.gameDone = FALSE

        #turn the field buttons on
        for line in self.buttons:
            for button in line:
                button.config(state=NORMAL)

        #if player is true the player starts otherwise the computer starts
        self.myLabel.config(text="You are H, make a move")
        if player:
            self.turn = FALSE
            self.second = TRUE
        else:
            self.turn = TRUE
            self.second = FALSE
            self.do_move()

    def do_button(self, i, j):
        """handle a field button click"""
        #i is the number of the button that was pushed note: 1 through 9
        # if turn is true the computer made a move otherwie player
        if self.turn:
            self.turn = FALSE
            self.bState[i][j] = -1
        else:
            self.turn = TRUE
            self.bState[i][j] = 1
        self.state += ("p|{}|{}|$".format(i, j))
        self.do_redraw();
        #Disable the ith button and test if we are done
        #self.buttons[i][j].config(text=myText, state=DISABLED)
        #self.test_done()

        # if it is the computer's turn and the game is not over make a move
        #if (not self.turn) and (not self.gameDone):
        self.do_move()

    def test_done(self):
        """test if the game is over"""
        #if there is not a 0 in bstate the game is done
        if not (0 in self.bState):
            self.myLabel.config(text="Draw, game over!")
            self.gameDone = TRUE
            self.again_button.config(state=NORMAL)

        #after doing sums look for 3 or -3 to find if there was a winner
        self.do_sums()
        if 3 in self.mySums: #note 3 in mySums means player has won
            self.myLabel.config(text="You won!")
            self.gameDone = TRUE
            self.again_button.config(state=NORMAL)
        elif -3 in self.mySums: #note -3 in mySums means computer has won
            self.myLabel.config(text="Computer won!")
            self.gameDone = TRUE
            self.again_button.config(state=NORMAL)

    def do_move(self):
        """computer picks a move to make"""
        moves = domove(self.state, bot=self.bot.get()).strip()
        self.state = self.state + moves
        moves = moves.split('$')
        turn = -1 if self.second == TRUE else 1
        if len(moves) == 1:
            self.bState[int(moves[0][-3])][int(moves[0][-1])] = turn
        elif len(moves) == 2 and moves[0].split('|')[0] == 'p' and moves[1].split('|')[0] == 'p':
            #second movement == 2 places
            self.bState[int(moves[0][-3])][int(moves[0][-1])] = turn
            self.bState[int(moves[1][-3])][int(moves[1][-1])] = turn
        else:
            #normal movement
            if moves[-8] == 'p':
                self.bState[int(moves[-7])][int(moves[-6])] = turn
                self.bState[int(moves[-4])][int(moves[-3])] = 0
                self.bState[int(moves[-2])][int(moves[-1])] = turn
            else:
                self.bState[int(moves[-7])][int(moves[-6])] = 0
                self.bState[int(moves[-5])][int(moves[-4])] = turn
                self.bState[int(moves[-2])][int(moves[-1])] = turn
        self.do_redraw()

    def do_again(self):
        """reset everything to play again"""
        #reset my buttons and change the label
        self.player_button.config(state=NORMAL)
        self.computer_button.config(state=NORMAL)
        self.myLabel.config(text="Who starts?")
        self.bState = [[0, 0, 0, 0, 0, 0, 0 ,0] for i in range(8)]
        self.state = ""

        #disable the field buttons
        for line in self.buttons:
            for button in line:
                button.config(text="---", state=DISABLED, background=self['bg'])

    def do_redraw(self):
        for i in range(8):
            for j in range(8):
                if self.bState[i][j] == -1:
                    self.buttons[i][j].config(text="", background='black', state=DISABLED)
                elif self.bState[i][j] == 1:
                    self.buttons[i][j].config(text="", background='white', state=DISABLED)

    def do_exit(self):
        """destroy the frame when exit is pushed"""
        root.destroy()


if __name__ == '__main__':
    root = SesquiGUI()
    root.title("SesquiGUI");
    root.mainloop()
