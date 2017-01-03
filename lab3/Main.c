code Main

  -- OS Class: Project 3
  --
  -- Wen Tao Zhao
  --

  -- This package contains the following:
  --     Dining Philospohers
-----------------------------  Main  ---------------------------------

  function main ()
	InitializeScheduler ()
	--DiningPhilosophers ()
	SleepingBarber ()
	--gameParlor()
       ThreadFinish ()
      FatalError ("Need to implement")
    endFunction

---------------------------------  Game Parlor  --------------------------------
var gp: GameParlor 
group: array [8] of Thread = new array of Thread {8 of new Thread}

function gameParlor()

gp = new GameParlor
gp.Init()

	group[0].Init("A Backgammo")
	group[0].Fork(Play, 4)

	group[1].Init("B Backgammo")
	group[1].Fork(Play, 4)

	group[2].Init("C Risk")
	group[2].Fork(Play, 5)

	group[3].Init("D Risk")
	group[3].Fork(Play, 5)

	group[4].Init("E Monopoly")
	group[4].Fork(Play, 2)

	group[5].Init("F Monopoly")
	group[5].Fork(Play, 2)

	group[6].Init("G Pictionary")
	group[6].Fork(Play, 1)

	group[7].Init("H Pictionary")
	group[7].Fork(Play, 1)
	
	ThreadFinish()
endFunction 

function Play (diceNeeded: int)
	var i: int

	for i = 1 to 5
	gp.Request(diceNeeded)
	
	currentThread.Yield()

	gp.Return(diceNeeded)
	endFor
endFunction

class GameParlor
	superclass Object
	fields
		numDiceAvail:int
		diceCount:Mutex
		requiresDice:Condition
	methods
		Init()
		Request(numNeeded:int)
		Return(numReturned:int)
		Print(str:String, count:int)
endClass

behavior GameParlor

method Init()
	numDiceAvail = 8
	requiresDice = new Condition
	requiresDice.Init()
	diceCount = new Mutex
	diceCount.Init()
endMethod

method Request(numNeeded:int)
	self.Print ("Requests", numNeeded)	
	diceCount.Lock()	--Locks the dice
	while numNeeded>numDiceAvail
		requiresDice.Wait(&diceCount)	--Waits until dice become available
	endWhile
	numDiceAvail = numDiceAvail - numNeeded	--Subtracts dice
	diceCount.Unlock()	--Unlocks the dice
	self.Print("proceeds with", numNeeded)
endMethod

method Return(numReturned:int)

	diceCount.Lock()	--Locks the dice
	numDiceAvail = numDiceAvail + numReturned	--Adds dice
	self.Print("Releases and adds back", numReturned)
	requiresDice.Signal(&diceCount)	--Signals diceCount
	diceCount.Unlock()	--Unlocks the dice
endMethod

method Print (str: String, count: int)
	--
	-- This method prints the current thread's name and the arguments.
	-- It also prints the current number of dice available.
	--
	var oldStatus: int
	oldStatus = SetInterruptsTo (DISABLED)
	print (currentThread.name)
	print (" ")
	print (str)
	print (" ")
	printInt (count)
	nl ()
	print ("------------------------------Number of dice now avail = ")
	printInt (numDiceAvail)
	nl ()
	oldStatus = SetInterruptsTo (oldStatus)
endMethod
endBehavior




-------------------------------  Sleeping Barber  ------------------------------

const CHAIRS = 5 --# chairs for waiting customers 
var 
custsem: Semaphore = new Semaphore	--number of customers waiting
haircutavailsem: Semaphore = new Semaphore	--number of haircuts ready
barbavailsem: Semaphore = new Semaphore	--number of barbers available 
mutex: Mutex = new Mutex	--mutex lock
waiting: int=0
thArray: array [11] of Thread = new array of Thread {11 of new Thread} 

function SleepingBarber()		
	custsem.Init (0)
	haircutavailsem.Init (0)
	barbavailsem.Init(1)
	mutex.Init ()
	print("          Barber   1   2   3   4   5   6   7   8   9   10")

	thArray[0].Init("")
	thArray[0].Fork(Barber,0)

	thArray[1].Init("              ")
	thArray[1].Fork(loop,1)

	thArray[2].Init("                  ")
	thArray[2].Fork(loop,2)

	thArray[3].Init("                      ")
	thArray[3].Fork(loop,3)

	thArray[4].Init("                          ")
	thArray[4].Fork(loop,4)

	thArray[5].Init("                              ")
	thArray[5].Fork(loop,5)

	thArray[6].Init("                                  ")
	thArray[6].Fork(loop,6)

	thArray[7].Init("                                      ")
	thArray[7].Fork(loop,7)

	thArray[8].Init("                                          ")
	thArray[8].Fork(loop,8)

	thArray[9].Init("                                              ")
	thArray[9].Fork(loop,9)

	thArray[10].Init("                                                  ")
	thArray[10].Fork(loop,10)
	ThreadFinish ()
endFunction

function loop(num: int)
var 
	i: int 
	for i = 0 to 2 
	Customer(num)
endFor 
endFunction


function Barber (myId: int)
while (true )
	barbavailsem.Down()
	custsem.Down()	--goes to sleep is no customers
	mutex.Lock()	--gets acess to "waiting"	
	waiting = waiting - 1	--decrements number of waiting customers
	
	printString("     Start")

	haircutavailsem.Up()	--barber is ready to cut hair
	mutex.Unlock()	--releases "waiting"
endWhile
endFunction

function Customer (myId: int)
mutex.Lock()	--critical region
printString("E")
if (waiting < CHAIRS)	--if no free chairs, leave
	waiting = waiting + 1	--one more waiting customer
	printString("S")
	custsem.Up()	--wakes up barber if necessary
	mutex.Unlock()	--release access to "waiting"
	haircutavailsem.Down()	--sleep if no haircuts ready
	
	printString("B")
	currentThread.Yield()
	printString("F")
	printChairs()
	print("     End")
	printString("L")
	barbavailsem.Up()
else 
	printString("L")
	mutex.Unlock()	--no seats, leave
endIf
endFunction

function printChairs()
	var i: int
	print("\n")
	for i = 1 to waiting  
		print("X")
	endFor
	for i = 1 to (CHAIRS - waiting)
		print("-")
	endFor
endFunction

function printString (str:String)
	var
	oldIntStat: int
	oldIntStat = SetInterruptsTo (DISABLED)	
	printChairs()
	print(currentThread.name)
	print(str)
	oldIntStat = SetInterruptsTo (oldIntStat)
endFunction


-----------------------------  Dining Philosophers  ----------------------------

  -- This code is an implementation of the Dining Philosophers problem.  Each
  -- philosopher is simulated with a thread.  Each philosopher thinks for a while
  -- and then wants to eat.  Before eating, he must pick up both his forks.
  -- After eating, he puts down his forks.  Each fork is shared between
  -- two philosophers and there are 5 philosophers and 5 forks arranged in a
  -- circle.
  --
  -- Since the forks are shared, access to them is controlled by a monitor
  -- called "ForkMonitor".  The monitor is an object with two "entry" methods:
  --     PickupForks (phil)
  --     PutDownForks (phil)
  -- The philsophers are numbered 0 to 4 and each of these methods is passed an 
  -- integer indicating which philospher wants to pickup (or put down) the forks.
  -- The call to "PickUpForks" will wait until both of his forks are
  -- available.  The call to "PutDownForks" will never wait and may also
  -- wake up threads (i.e., philosophers) who are waiting.
  --
  -- Each philospher is in exactly one state: HUNGRY, EATING, or THINKING.  Each
  -- time a philosopher's state changes, a line of output is printed.  The 
  -- output is organized so that each philosopher has column of output with the
  -- following code letters:
  --           E    --  eating
  --           .    --  thinking
  --         blank  --  hungry (i.e., waiting for forks)
  -- By reading down a column, you can see the history of a philosopher.
  --
  -- The forks are not modeled explicitly.  A fork is only picked up
  -- by a philosopher if he can pick up both forks at the same time and begin
  -- eating.  To know whether a fork is available, it is sufficient to simply
  -- look at the status's of the two adjacent philosophers.  (Another way to 
  -- state the problem is to forget about the forks altogether and stipulate 
  -- that a philosopher may only eat when his two neighbors are not eating.)

  enum HUNGRY, EATING, THINKING
  var
    mon: ForkMonitor
    philospher: array [5] of Thread = new array of Thread {5 of new Thread }

  function DiningPhilosophers ()

      print ("Plato\n")
      print ("    Sartre\n")
      print ("        Kant\n")
      print ("            Nietzsche\n")
      print ("                Aristotle\n")

      mon = new ForkMonitor
      mon.Init ()
      mon.PrintAllStatus ()

      philospher[0].Init ("Plato")
      philospher[0].Fork (PhilosphizeAndEat, 0)

      philospher[1].Init ("Sartre")
      philospher[1].Fork (PhilosphizeAndEat, 1)

      philospher[2].Init ("Kant")
      philospher[2].Fork (PhilosphizeAndEat, 2)

      philospher[3].Init ("Nietzsche")
      philospher[3].Fork (PhilosphizeAndEat, 3)

      philospher[4].Init ("Aristotle")
      philospher[4].Fork (PhilosphizeAndEat, 4)

     endFunction

  function PhilosphizeAndEat (p: int)
    -- The parameter "p" identifies which philosopher this is.
    -- In a loop, he will think, acquire his forks, eat, and
    -- put down his forks.
      var
        i: int
      for i = 1 to 7
        -- Now he is thinking
        mon.PickupForks (p)
        -- Now he is eating
        mon.PutDownForks (p)
      endFor
    endFunction

  class ForkMonitor
    superclass Object
    fields
      status: array [5] of int 	-- For each philosopher: HUNGRY, EATING, or THINKING
	monLock: Mutex 	--Monitor lock
	canEat: array [5] of Condition 	-- Whether or not each philospher can eat
    methods
      Init ()
      PickupForks (p: int)
      PutDownForks (p: int)
      PrintAllStatus ()
	CheckNeighbours(p:int)
  endClass

  behavior ForkMonitor

    method Init ()
      -- Initialize so that all philosophers are THINKING.
      -- Initializes the monitor lock and condition variables 
	var i:int 
	status = new array of int {5 of THINKING}
	canEat = new array [5] of Condition {5 of new Condition}
	monLock = new Mutex
	monLock.Init()	
	for i=0 to 4
		canEat[i].Init() 
	endFor

      endMethod

    method PickupForks (p: int)
      -- This method is called when philosopher 'p' wants to eat.
	monLock.Lock()	--locks 
	status[p] = HUNGRY	--status set to hungry
	self.PrintAllStatus()	--prints all status 
	self.CheckNeighbours (p)	--sees if he can start eating
	if status[p] != EATING		--if he cant, he waits	
		canEat[p].Wait (& monLock) --.Wait takes in a pointer to a Mutex lock
	endIf
	monLock.Unlock()--unlocks
      endMethod

    method PutDownForks (p: int)
      -- This method is called when the philosopher 'p' is done eating.
	monLock.Lock()
	status [p] = THINKING
	self.PrintAllStatus()
	self.CheckNeighbours ((p-1)%5)	--Checks if putting down forks lets his neighbours 
	self.CheckNeighbours ((p+1)%5)	--available to eat
	monLock.Unlock()
      endMethod

    method CheckNeighbours (p: int)
      -- This method checks if the philosopher 'p' can start eating
	
	if status[p] == HUNGRY && status [(p-1)%5] != EATING && status[(p+1)%5] != EATING
		status[p] = EATING
		self.PrintAllStatus()
		canEat[p].Signal (&monLock)
	endIf
      endMethod

    method PrintAllStatus ()
      -- Print a single line showing the status of all philosophers.
      --      '.' means thinking
      --      ' ' means hungry
      --      'E' means eating
      -- Note that this method is internal to the monitor.  Thus, when
      -- it is called, the monitor lock will already have been acquired
      -- by the thread.  Therefore, this method can never be re-entered,
      -- since only one thread at a time may execute within the monitor.
      -- Consequently, printing is safe.  This method calls the "print"
      -- routine several times to print a single line, but these will all
      -- happen without interuption.
        var
          p: int
        for p = 0 to 4
          switch status [p]
            case HUNGRY:
              print ("    ")
              break
            case EATING:
              print ("E   ")
              break
            case THINKING:
              print (".   ")
              break
          endSwitch
        endFor
        nl ()
      endMethod

  endBehavior

endCode
