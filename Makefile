OUT = chess
CXX = g++
CXXFLAGS = -g -Wall -std=c++11
SRCFILES = chess.cc game.cc player.cc piece.cc board.cc message.cc view.cc
OFILES = $(SRCFILES:%.cc=%.o)

all: $(OUT)
	@rm -f *.o

$(OUT): $(OFILES)
	$(CXX) $(OFILES) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
		egrep -v "/usr/include") > Makefile.new
	@mv Makefile.new Makefile

#
# -- Regles de dependances generees automatiquement
#
# DO NOT DELETE THIS LINE
chess.o: chess.cc message.h common.h view.h game.h player.h piece.h
game.o: game.cc player.h piece.h common.h board.h message.h view.h game.h
player.o: player.cc player.h piece.h common.h board.h message.h
piece.o: piece.cc piece.h common.h board.h
board.o: board.cc board.h common.h view.h
message.o: message.cc message.h common.h view.h
view.o: view.cc view.h common.h
