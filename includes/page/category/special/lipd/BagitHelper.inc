<?php

require_once 'bagit/lib/bagit.php';

class BagitHelper {
	public static function tempdir() {
    	$tempfile=tempnam(sys_get_temp_dir(),'');
    	if (file_exists($tempfile)) 
			unlink($tempfile);
    	mkdir($tempfile);
    	if (is_dir($tempfile)) 
			return $tempfile; 
	}

	public static function removeDir( $tempdir ) {
		if(!$tempdir) return;

		$files = array_diff(scandir($tempdir), array('.','..')); 
		foreach ($files as $file) { 
			(is_dir("$tempdir/$file")) ? BagitHelper::removeDir("$tempdir/$file") : unlink("$tempdir/$file"); 
		} 
    	return rmdir($tempdir); 
	}
	
	public static function extractLiPD( $file ) {
		$target = BagitHelper::tempdir();

		$bag = new BagIt($file);
		$bag->validate();

		if (count($bag->getBagErrors()) == 0) {
  			$bag->fetch->download();
  			foreach ($bag->getBagContents() as $filename) {
		    	copy($filename, "$target/" . basename($filename));
  			}
		}

		return $target;
	}

	public static function createLiPD( $name, $dir ) {
		$tmpdir = BagitHelper::tempdir();
		$basedir = "$tmpdir/$name";
		mkdir($basedir);

		$lipdzip = "$basedir.zip";
		$lipdfile = "$basedir.lpd";

		$bag = new BagIt($basedir);

		$files = array_diff(scandir($dir), array('.','..')); 
		foreach ($files as $file) { 
			if(!is_dir("$dir/$file")) 
				$bag->addFile("$dir/$file", $file);
		} 
		$bag->update();

		$bag->package($basedir, "zip");
		rename($lipdzip, $lipdfile);

		BagitHelper::removeDir($basedir);
		
		return $lipdfile;
	}
}
