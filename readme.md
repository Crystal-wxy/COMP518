Homework 2
----------

1. Recall from the lectures the approximation algorithm for counting 1's over a sliding window.

2. Implement this approximation algorithm in `window-bit-count-apx/window-bit-count-apx.h`.

3. Test your implementation using `window-bit-count-apx/test.c`.

4. Keep in mind that the tests in `test.c` are not comprehensive. It is possible that the tests pass, but the implementation is incorrect.

5. Benchmark your implementation by typing `make` inside the directory `window-bit-count-plots`. You will need to download R (https://www.r-project.org/) in order to create the plots.

6. If you feel that there are certain aspects of your implementation that need explanation, you can create a short report and submit it with the homework. Alternatively, you can include comments in the code.

7. Your implementation **must** be as space-efficient (asymptotically) as the algorithm presented in the lectures.

8. **OPTIONAL**: We will rank the submitted solutions in terms of time efficiency (using throughput and worst-case time-per-item). We will consider giving extra credit (up to 30% extra) to some of the best-performing solutions. Credit will be allocated on the basis of the implementation ideas that improve the throughput. In order to get extra credit, you should clearly explain the ideas you have used to improve the throughput.