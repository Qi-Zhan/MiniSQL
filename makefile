CC = g++
OBJS = Interpreter.o API.o BufferManager.o head.o CatalogManager.o RecordManager.o BPlusTree.o IndexManager.o 
MiniSQL: $(OBJS)
	$(CC) $(OBJS) -o $@
clean:
	-rm $(OBJS) MiniSQL