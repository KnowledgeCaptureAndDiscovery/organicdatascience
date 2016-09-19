<?php

class Util {
  public static function readFile($filename) {
    $rval = @file_get_contents($filename);
    if($rval === false) {
      throw new Exception('File read error: ' . $filename);
    }
    return $rval;
  }
  public static function readJson($filename) {
    $rval = json_decode(self::readFile($filename), true);
    if($rval === null) {
      throw new Exception('JSON parse error');
    }   
    return $rval;
  }     
  public static function readNQuads($filename) {
    return self::readFile($filename);
  }
  public static function jsonldEncode($input) {
    // newer PHP has a flag to avoid escaped '/'
    if(defined('JSON_UNESCAPED_SLASHES')) {
      $options = JSON_UNESCAPED_SLASHES;
      if(defined('JSON_PRETTY_PRINT')) {
        $options |= JSON_PRETTY_PRINT;
      }
      $json = json_encode($input, $options);
    } else {
      // use a simple string replacement of '\/' to '/'.
      $json = str_replace('\\/', '/', json_encode($input));
    }
    return $json;
  }

  public static function is_assoc($arr) {
    return is_array($arr) && (array_keys($arr) !== range(0, count($arr) - 1));
  }
}
?>
