<?php

require_once("LiPDJson.inc");
require_once("LiPDWiki.inc");

class WTLiPD extends SpecialPage {
	
	public function __construct() {
		parent::__construct( 'WTLiPD' );
	}

	function execute( $par ) {
		global $wgDir;
		$request = $this->getRequest();
		$out = $this->getOutput();
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
		$this->setHeaders();
 
		$this->handleOperation($request, $out);
		$this->showImportForm($out);
		$this->showDatasets("LiPD", $out);
	}

	function handleOperation($request, $out) {
		$op = $request->getText("op");
		$loc = preg_replace("/\?.*$/", "", $request->getFullRequestURL());
		switch($op) {
			case "import":
				$num = $this->importLiPD($out);
				if($num)
					$request->response()->header("Location: $loc");
				break;
			case "export":
				$lipdid = $request->getText("lipdid");
				$this->exportLiPD($lipdid, $request, $out);
				break;
			case "delete":
				$lipdid = $request->getText("lipdid");
				$this->deleteLiPD($lipdid, $out);
				$request->response()->header("Location: $loc");
				break;
		}
	}

	function importLiPD( $out ) {
		if(array_key_exists("files", $_FILES)) {
			$files = $_FILES["files"];

			$i=0;
			$processed_count=0;
			foreach ($files['tmp_name'] as $file) {
				$name = $files['name'][$i];
				$importer = new LiPDJson($name, $file);
				$ok = $importer->import($out);
				if($ok)
					$processed_count++;
				$i++;
			}

			$out->addHtml("<br/>Processed $processed_count LiPD files<br/><br />\n");
			return $processed_count;
		}
		return 0;
	}

	function exportLiPD($dsid, $request, $out) {
		$exporter = new LiPDWiki($dsid);
		$lipdfile = $exporter->export($out);

		$request->response()->header('Content-Description: File Transfer');
		$request->response()->header('Content-Type: application/octet-stream');
		$request->response()->header('Content-Disposition: attachment; filename="'.basename($lipdfile).'"');
		$request->response()->header('Expires: 0');
		$request->response()->header('Cache-Control: must-revalidate');
		$request->response()->header('Pragma: public');
		$request->response()->header('Content-Length: ' . filesize($lipdfile));
		readfile($lipdfile);

		unlink($lipdfile);
		exit;
	}

	function deleteLiPD($dsid, $out) {
		$deleter = new LiPDWiki($dsid);
		$deleter->delete($out);
	}

	function showImportForm( $out ) {
		// Show the form
		$html  = "<h3>Import LiPD Datasets</h3>\n";
		$html .= "<form method='post' enctype='multipart/form-data'>\n";
		$html .= "Select file/s: <input type='file' name='files[]' multiple /> \n";
		$html .= "<input type='hidden' name='op' value='import' />\n";
		$html .= "<input type='submit' />\n";
		$html .= "</form>\n";
		$out->addHtml($html);
	}

	function showDatasets( $category, $out ) {
		$query = "[[Category:$category]]";
		$rawparams = array($query);

		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
        SMWQueryProcessor::addThisPrintout( $printouts, $params );

        $m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
        $queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);

        $results = smwfGetStore()->getQueryResult( $queryobj );

		$out->addHTML("<h3>Current LiPD Datasets</h3>\n");
		$out->addHTML("<table class='sortable wikitable' width='100%'>\n");
		$out->addHTML("<tr><th>LiPD Dataset</th><th class='unsortable' /></tr>\n");
        $res = $results->getNext();
        while($res != null) {
			$lipdpage = $res[0]->getNextObject();
			$id = $lipdpage->getDbKey();
			$out->addHTML("<tr>\n");
			$out->addHTML("<td class='smwtype_wpg'><a href='$id'>$id</a></td>\n");
			$out->addHTML("<td>\n");
			$out->addHTML("<a class='lodbutton x-small' href='?op=export&lipdid=$id'>Get LiPD</a>\n");
			$out->addHTML("<a class='lodbutton x-small' onclick='return confirm(\"Are you sure you want to delete \\n$id\");' ");
			$out->addHTML("href='?op=delete&lipdid=$id'>Delete</a>\n");
			$out->addHTML("</td>\n");
			$out->addHTML("</tr>\n");
			$res = $results->getNext();
		}
		$out->addHTML("</table>\n");
	}

}

