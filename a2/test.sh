#make clean
make

./solver samples/input1 > input1.out
diff input1.out samples/input1.out
if [ $? -eq 0 ]
then
    echo "Test 1 passed"
else
    echo "Test 1 failed"
fi

./solver samples/input2 > input2.out
diff input2.out samples/input2.out
if [ $? -eq 0 ]
then
    echo "Test 2 passed"
else
    echo "Test 2 failed"
fi

./solver samples/input3 > input3.out
diff input3.out samples/input3.out
if [ $? -eq 0 ]
then
    echo "Test 3 passed"
else
    echo "Test 3 failed"
fi

./solver samples/input4 > input4.out
diff input4.out samples/input4.out
if [ $? -eq 0 ]
then
    echo "Test 4 passed"
else
    echo "Test 4 failed"
fi

./solver samples/input5 > input5.out
diff input5.out samples/input5.out
if [ $? -eq 0 ]
then
    echo "Test 5 passed"
else
    echo "Test 5 failed"
fi

./solver samples/input6 > input6.out
diff input6.out samples/input6.out
if [ $? -eq 0 ]
then
    echo "Test 6 passed"
else
    echo "Test 6 failed"
fi

./solver samples/input8 > input8.out
diff input8.out samples/input8.out
if [ $? -eq 0 ]
then
    echo "Test 8 passed"
else
    echo "Test 8 failed"
fi

make clean
rm -f ./*.out
