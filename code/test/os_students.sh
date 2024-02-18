make distclean
make 
# ../build.linux/nachos -e halt
echo "Running L3 test_1"
timeout 1 ../build.linux/nachos -ep hw4_normal_test1 0 -ep hw4_normal_test2 0 
echo "L3 test_1 done"
echo "================================================"

echo "Running L2 test_1"
timeout 1 ../build.linux/nachos -ep hw4_normal_test1 50 -ep hw4_normal_test2 50
echo "Running L2 test_1 done"
echo "================================================"

echo "Running L2 test_2"
timeout 1 ../build.linux/nachos -ep hw4_normal_test1 50 -ep hw4_normal_test2 90
echo "Running L2 test_2 done"
echo "================================================"

echo "Running L2 test_3"
timeout 1 ../build.linux/nachos -ep hw4_normal_test1 70 -ep hw4_normal_test2 80 -ep hw4_normal_test3 50
echo "Running L2 test_3 done"
echo "================================================"

echo "Running L1 test_1"
timeout 1 ../build.linux/nachos -ep hw4_normal_test1 100 -ep hw4_normal_test2 100
echo "Running L1 test_1 done"
echo "================================================"

echo "Running L1 test_2"
timeout 3 ../build.linux/nachos -ep hw4_delay_test1 100 -ep hw4_normal_test2 100
echo "Running L1 test_2 done"
echo "================================================"

echo "Running Aging L3 -> L2 test_1"
timeout 3 ../build.linux/nachos -ep hw4_delay_test1 50 -ep hw4_normal_test2 45
echo "Running Aging L3 -> L2 test_1 done"
echo "================================================"

