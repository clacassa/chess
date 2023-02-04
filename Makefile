OUT = chess
CXX = g++
CXXFLAGS = -time -g -Wall -std=c++11 -I ./include/
SRCFILES = chess.cc game.cc player.cc piece.cc board.cc view.cc
OFILES = $(SRCFILES:%.cc=%.o)

all: $(OUT)
	@rm -f *.o

$(OUT): $(OFILES)
	$(CXX) $(OFILES) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

#
# -- Regles de dependances generees automatiquement
#
# DO NOT DELETE THIS LINE
chess.o: view.h common.h game.h player.h piece.h
game.o: player.h piece.h common.h board.h view.h game.h
player.o: player.h piece.h common.h board.h
piece.o: piece.h common.h board.h
board.o: board.h common.h view.h
view.o: view.h common.h

.PHONY: dev
dev: $(OUT) 

.PHONY: clean
clean:
	@rm -f *.o $(OUT)

.PHONY: depend
depend:
	@echo " *** MISE A JOUR DES DEPENDANCES ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(SRCFILES) | \
	  egrep -v "/usr/include") >Makefile.new
	@mv Makefile.new Makefile
