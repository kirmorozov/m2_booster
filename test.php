<?php
$counter = new \Some\Counter();
$counter->increment(5);
$counter->increment();
$counter->decrement(3);
echo($counter->value()."\n");

// $dataObj = new \Magento\Framework\DataObject();
$dataObj = new \Magento\Framework\DataObject(['ok'=>'val', 'ok2'=>['ok3']]);
$dataObj->setData('str1', 'ok');
$dataObj->setData([
    'str2'=> 'ok2',
    'str3'=> 'ok2'
    ]);
$dataObj->addData([
    'str1'=> 'ok1',
    'str3'=> 'ok33'
    ]);

$dataObj->addData([
    'str13'=> 'ok1',
    'str33'=> 'ok33'
    ]);
$dataObj->setData('str81', 'ok');
$dataObj->setData('str71', 'ok');
$dataObj->setData('str61', 'ok');
$dataObj->setData('str51', 'ok');
$dataObj->setData('str41', 'ok');
$dataObj->setData('str41', 'ok');
$dataObj->setData('str31', 'ok');

echo $dataObj->_vall() .'OK';

$dataObj->unsetData('str31');

echo $dataObj->_vall() .'OK';
?>