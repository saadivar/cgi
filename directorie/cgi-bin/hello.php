<!-- <?php


#echo "Name: <br>";
// while(true);
//  if ($_SERVER["REQUEST_METHOD"] === "POST") {
// //     // Retrieve the posted data
//      $name = $_POST["name"];
//      $title = $_POST["title"];

// //     // Process the data (e.g., print it)
// //     // header("Content-Type: text/html; charset=UTF-8");
//     echo "<html><body>";
//     echo "<h1>Received Data:</h1>";
//     echo "<p>Name: " . htmlspecialchars($name) . "</p>";
//     echo "<p>Title: " . htmlspecialchars($title) . "</p>";
//     echo "</body></html>";
// } else {
//     // Handle other HTTP methods or provide an error message
//     header("HTTP/1.1 405 Method Not Allowed");
//     echo "Method not allowed";
// } 
// print_r($_POST);

// print_r($_GET);

// ?>   -->

<?php

$contentTypeToExtension = [
    'image/jpeg' => '.jpg',
    'image/png' => '.png',
    'image/gif' => '.gif',
    'video/mp4' => '.mp4',
    'video/quicktime' => '.mov',
    'application/pdf' => '.pdf',
    'application/msword' => '.doc',
    'application/vnd.openxmlformats-officedocument.wordprocessingml.document' => '.docx',
    'application/vnd.ms-excel' => '.xls',
    'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet' => '.xlsx',
    'application/zip' => '.zip',
];

$contentType = isset($_SERVER['CONTENT_TYPE']) ? $_SERVER['CONTENT_TYPE'] : '';
$fileExtension = isset($contentTypeToExtension[$contentType]) ? $contentTypeToExtension[$contentType] : '';

$inputData = file_get_contents("php://input");
$filename = "../upload/" . bin2hex(random_bytes(8)) . $fileExtension;

file_put_contents($filename, $inputData);
if (file_exists($filename)) {
    echo "Data written to $filename successfully.";
} else {
    echo "Failed to write data to $filename.";
}

?>


