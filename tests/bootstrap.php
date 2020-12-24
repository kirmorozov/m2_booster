<?php
include dirname(__DIR__).'/vendor/autoload.php';
if (!class_exists('Magento\Framework\DataObject')) {
    echo "Time for original\r\n";
    include __DIR__."/src/DataObject.php";
}
