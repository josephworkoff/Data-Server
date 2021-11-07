SRCDIR=src
BUILDDIR=build
BINDIR=bin
INCLUDEDIR=include
DATADIR=data
LOGSDIR=logs
INC=-Iinclude

SERVEREXE=bin/server
CLIENTEXE=bin/client


all: $(SERVEREXE) $(CLIENTEXE)

$(CLIENTEXE): $(BUILDDIR)/maincli.o $(BUILDDIR)/SocketConnection.o $(BUILDDIR)/Client.o $(BUILDDIR)/CriticalFile.o  $(BUILDDIR)/SharedMemory.o $(BUILDDIR)/SemaphoreSet.o
	@mkdir -p $(BINDIR)
	@mkdir -p $(LOGSDIR)
	g++ -o  $(CLIENTEXE) $(INC) $(BUILDDIR)/maincli.o $(BUILDDIR)/SocketConnection.o $(BUILDDIR)/Client.o $(BUILDDIR)/SharedMemory.o $(BUILDDIR)/SemaphoreSet.o $(BUILDDIR)/CriticalFile.o 

$(SERVEREXE): $(BUILDDIR)/mainser.o $(BUILDDIR)/SocketConnection.o $(BUILDDIR)/Server.o $(BUILDDIR)/CriticalFile.o $(SRCDIR)/CriticalFile.cpp  $(BUILDDIR)/SemaphoreSet.o
	@mkdir -p $(BINDIR)
	@mkdir -p $(LOGSDIR)
	g++ -o $(SERVEREXE) $(INC) $(BUILDDIR)/mainser.o $(BUILDDIR)/Server.o $(BUILDDIR)/SocketConnection.o $(BUILDDIR)/SemaphoreSet.o $(BUILDDIR)/CriticalFile.o 

$(BUILDDIR)/maincli.o: $(SRCDIR)/maincli.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/maincli.cpp 

$(BUILDDIR)/mainser.o: $(SRCDIR)/mainser.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/mainser.cpp

$(BUILDDIR)/Server.o: $(INCLUDEDIR)/Server.h $(SRCDIR)/Server.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/Server.cpp

$(BUILDDIR)/Client.o: $(INCLUDEDIR)/Client.h $(SRCDIR)/Client.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/Client.cpp
	
$(BUILDDIR)/SocketConnection.o: $(INCLUDEDIR)/SocketConnection.h $(SRCDIR)/SocketConnection.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/SocketConnection.cpp

$(BUILDDIR)/CriticalFile.o: $(INCLUDEDIR)/CriticalFile.h $(SRCDIR)/CriticalFile.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/CriticalFile.cpp

$(BUILDDIR)/SharedMemory.o: $(INCLUDEDIR)/SharedMemory.h $(SRCDIR)/SharedMemory.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/SharedMemory.cpp

$(BUILDDIR)/SemaphoreSet.o: $(INCLUDEDIR)/SemaphoreSet.h $(SRCDIR)/SemaphoreSet.cpp
	@mkdir -p $(BUILDDIR)
	g++ -c -o $@ $(INC) $(SRCDIR)/SemaphoreSet.cpp

clean:
	rm -rf $(BUILDDIR) $(BINDIR) $(LOGSDIR) $(SERVEREXE) $(CLIENTEXE)
	cp $(DATADIR)/ref.bin $(DATADIR)/out.bin