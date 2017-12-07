<?php

class CSVImport extends SpecialPage {

	public function __construct() {
		parent::__construct( 'CSVImport' );
	}

  function execute( $par ) {
		global $wgDir, $wgUser;
		$request = $this->getRequest();
		$out = $this->getOutput();
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
		$this->setHeaders();
		$this->handleOperation($request, $out);
		if($wgUser->isAllowedAll("createpage", "edit-page-metadata"))
			$this->showImportForm($out);
		//$this->showDatasets($out);
	}

	function handleOperation($request, $out) {
		global $wgUser;
		$op = $request->getText("op");
		$loc = preg_replace("/\?.*$/", "", $request->getFullRequestURL());
		switch($op) {
			case "import":
				if(!$wgUser->isAllowedAll("createpage", "edit-page-metadata"))
					break;
				$ok = $this->importCSV($out);
				/*if($ok)
					$request->response()->header("Location: $loc");*/
				break;
		}
	}

	function importCSV( $out ) {
		global $wgCore, $IP;
		require_once("$IP/maintenance/runJobs.php");

		if(array_key_exists("files", $_FILES)) {
			$files = $_FILES["files"];
			$i=0;
			foreach ($files['tmp_name'] as $file) {
				$name = $files['name'][$i];
        $catname = ucfirst(preg_replace("/\.csv$/i", "", $name));
        $out->addHTML("<h3>Import into Category: $catname</h3>");

				$properties = array();
        $csv = $this->readCSV($file);
				foreach($csv as $row) {
					if(array_key_exists($catname, $row)) {
						$id = $row[$catname];
						$data = $this->createPage($id, "${catname}_$wgCore", [], $out);
						foreach($row as $key=>$value) {
							$key = ucfirst($key);
							if($key != $catname) {
								$property = null;
								if(array_key_exists($key, $properties)) {
									$property = $properties[$key];
								}
								else {
									$propDI = $this->resolveProperty($key);
									if($propDI) {
										$details = WTFactsAPI::getPropertyDetails($propDI->getKey());
										$property = array("obj"=>$propDI, "details"=>$details);
									} else {
										$out->addHTML("<li>[Warning] $name: Column $key doesn't resolve to an existing property");
									}
									$properties[$key] = $property;
								}
								if($property) {
									$details = $property["details"];
									$values = preg_split("/\s*;\s*/", $value);
									foreach($values as $subvalue) {
										if($details["type"] == "_wpg") {
											$this->createPage($subvalue, $details["category"], $out);
										}
										$data = $this->setProperty( $data, $property["obj"], $subvalue);
									}
								}
							}
						}
						$this->savePage( $id, $data, $out );
					}
					else {
						$out->addHTML("<li>[Error] $name: Column $catname doesn't exist");
					}
				}
				$out->addHTML("<li><b>$name processed.</b>");
				$i++;
			}
			$out->addHTML("<br/>Running background jobs to update semantics..");
			$jobs = new RunJobs();
			$jobs->execute();
			$out->addHTML("<br/>..Done");
		}
	}

	private function createPage($objid, $category, $out) {
		$title = Title::newFromText($objid);
		if(!$title)
			return;

		$subject = SMWDIWikiPage::newFromTitle( $title );
		if(!$title->exists()) {
			$text = $category ? "[[Category:$category]]\n" : "";
			$summary = "CSVImport: New page $objid" . ($category ? " with Category: $category" : "");
			$ret = WTFactsAPI::createWikiPage($objid, $text, $summary);
			if($ret[0] != "Success")
				return null;
			return new SMWSemanticData($subject);
		}
		else {
			return smwfGetStore()->getSemanticData($subject);
		}
	}

	private function resolveProperty($name) {
		global $wgCore;
		$corename = $name."_$wgCore";
		if($this->pageExists("Property:$corename"))
			return SMWDIProperty::newFromUserLabel( $corename );
		if($this->pageExists("Property:$name"))
			return SMWDIProperty::newFromUserLabel( $name );
		return null;
	}

	private function setProperty( $data, $propDI, $value ) {
		if(!$data)
			return $data;

		$objid = $data->getSubject()->getTitle();
		$prop = $propDI->getLabel();

        $valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
        $valDI = $valDV->getDataItem();
        if($valDI->getDIType() == SMWDataItem::TYPE_ERROR)
            return $data;

        $data = WTFactsAPI::addPropValToData($data, $propDI, $valDI, false);

		return $data;
	}

	private function pageExists($id) {
		$title = Title::newFromText($id);
		return $title && $title->exists();
	}

	private function savePage( $objid, $data, $out ) {
		//$out->addWikiText("Saving $objid\n".$this->getDataAsString($data));
		WTFactsAPI::updateWiki( $objid, "CSVImport: Edit Page", $data );
	}

  function readCSV ($file) {
    $rows   = array_map('str_getcsv', file($file));
    $header = array_shift($rows);
    $csv    = array();
    foreach($rows as $row) {
        $csv[] = array_combine($header, $row);
    }
    return $csv;
  }

  function showImportForm( $out ) {
		// Show the form
		$html  = "<h3>Import CSV Data</h3>\n";
		$html .= "<form method='post' enctype='multipart/form-data'>\n";
		$html .= "Select file/s: <input type='file' name='files[]' multiple /> \n";
		$html .= "<input type='hidden' name='op' value='import' />\n";
		$html .= "<input type='submit' />\n";
		$html .= "</form>\n";
		$out->addHtml($html);
	}

	function getGroupName() {
		return "ODSGroup";
	}

}
