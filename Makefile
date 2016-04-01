all:
	echo "not done yet!"
clean:
	if [ -a *~ ] ; \
	then \
		rm *~ ;\
	fi;
	if [ -a *.o ] ; \
	then \
		rm *.o ;\
	fi;
	if [ -a *.x ] ; \
	then \
		rm *.x ;\
	fi;
cleansongs:
	rm songs/*~