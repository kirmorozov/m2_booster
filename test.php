<?php
// $counter = new \Some\Counter();
// $counter->increment(5);
// $counter->increment();
// $counter->decrement(3);
// echo($counter->value()."\n");

// $dataObj = new \Magento\Framework\DataObject();
$dataObj = new \Magento\Framework\DataObject(['ok'=>'val', 'ok2'=>['ok3'=>'kk']]);
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
$dataObj->setData('str81', 'ok2');
$dataObj->setData('str71', 'ok3');
$dataObj->setData('str61', 'ok4');
$dataObj->setData('str51', 'ok5');
$dataObj->setData('str41', 'ok');
$dataObj->setData('str41', 'ok');
$dataObj->setData('str31', 'ok');

echo $dataObj->_vall() .'OK';

echo "OKK". $dataObj->getData('ok2/ok3')."okkk\r\n";

$dataObj->unsetData('str31');

echo $dataObj->_vall() .'OK';


$store = new \Magento\Framework\DataObject(array (
    'store_id' => '3',
    'code' => 'new_theme',
    'website_id' => '3',
    'group_id' => '3',
    'name' => 'English',
    'sort_order' => '0',
    'is_active' => '1',
));
var_dump(
    $store->getData()
);

?>