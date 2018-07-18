.utl.getStats:{[]
				table: ("II"; enlist ",") 0:`orderDelaysStats.csv;
				t:select cnt:count OrderSeqenceId by delayInMilliSeconds from table;
				t:`cnt xdesc t;
				t
			  }

5#.utl.getStats[]
\\
