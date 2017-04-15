<?php

class WTLiPD extends SpecialPage {

	public function __construct() {
		parent::__construct( 'WTLiPD' );
	}

	function execute( $par ) {
		global $wgDir, $wgUseLiPD, $wgUser;
		$request = $this->getRequest();
		$out = $this->getOutput();
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
		$this->setHeaders();

		if(!$wgUseLiPD) {
			$out->addHTML("LiPD extension is not supported on this wiki. Set \$wgUseLiPD to true");
			return;
		}

		$this->handleOperation($request, $out);
		if($wgUser->isAllowedAll("createpage", "edit-page-metadata"))
			$this->showImportForm($out);
		$this->showDatasets($out);
	}

	function handleOperation($request, $out) {
		global $wgUser;
		$op = $request->getText("op");
		$loc = preg_replace("/\?.*$/", "", $request->getFullRequestURL());
		switch($op) {
			case "import":
				if(!$wgUser->isAllowedAll("createpage", "edit-page-metadata"))
					break;
				$ok = $this->importLiPD($out);
				if($ok)
					$request->response()->header("Location: $loc");
				break;
			case "export":
				$lipdid = $request->getText("lipdid");
				$this->exportLiPD($lipdid, $request, $out);
				break;
			case "exportAll":
				$this->exportAllLiPD($request, $out);
				break;
			case "delete":
				if(!$wgUser->isAllowedAll("delete"))
						break;
				$lipdid = $request->getText("lipdid");
				$this->deleteLiPD($lipdid, $out);
				$request->response()->header("Location: $loc");
				break;
			case "deleteAll":
				if(!$wgUser->isAllowedAll("delete"))
						break;
				$this->deleteAllLiPD($out);
				$request->response()->header("Location: $loc");
				break;
		}
	}

	function importLiPD( $out ) {
		require_once("LiPDJson.inc");
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
			return ($processed_count == sizeof($_FILES));
		}
		return false;
	}

	function exportLiPD($dsid, $request, $out) {
		require_once("LiPDWiki.inc");

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

	function exportAllLiPD($request, $out) {
		require_once("LiPDWiki.inc");

		$zip = new ZipArchive();
		$zipfile = sys_get_temp_dir() . "/LiPD-Archive-" . uniqid() . ".zip";
		if ($zip->open($zipfile, ZipArchive::CREATE)!==TRUE) {
		    exit("Cannot open <$zipfile>\n");
		}
		$pageids = $this->getAllDatasets();
		$lipdfiles = array();
		foreach($pageids as $dsid) {
			$exporter = new LiPDWiki($dsid);
			$lipdfile = $exporter->export($out);
			$zip->addFile($lipdfile, basename($lipdfile));
			$lipdfiles[] = $lipdfile;
		}
		$zip->close();

		$request->response()->header('Content-Description: File Transfer');
		$request->response()->header('Content-Type: application/octet-stream');
		$request->response()->header('Content-Disposition: attachment; filename="'.basename($zipfile).'"');
		$request->response()->header('Expires: 0');
		$request->response()->header('Cache-Control: must-revalidate');
		$request->response()->header('Pragma: public');
		$request->response()->header('Content-Length: ' . filesize($zipfile));
		readfile($zipfile);

		foreach($lipdfiles as $lipdfile)
			unlink($lipdfile);
		unlink($zipfile);
		exit;
	}

	function deleteLiPD($dsid, $out) {
		require_once("LiPDWiki.inc");

		$deleter = new LiPDWiki($dsid);
		$deleter->delete($out);
	}

	function deleteAllLiPD($out) {
		$pageids = $this->getAllDatasets();
		foreach($pageids as $id)
			$this->deleteLiPD($id, $out);
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

	function getAllDatasets() {
		global $wgCore;
		return $this->getAllPagesInCategory( "Dataset_$wgCore" );
	}

	function getAllPagesInCategory( $category ) {
		$query = "[[Category:$category]]";
		$rawparams = array($query, "limit=500");

		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);

		$results = smwfGetStore()->getQueryResult( $queryobj );

        $res = $results->getNext();
		$pageids = array();
        while($res != null) {
			$lipdpage = $res[0]->getNextObject();
			$id = $lipdpage->getDbKey();
			$pageids[] = $id;
			$res = $results->getNext();
		}
		return $pageids;
	}

	function showDatasets( $out ) {
		global $wgUser;
		$del = $wgUser->isAllowed("delete");
		$out->addHTML("<h3>Current LiPD Datasets</h3>\n");
		$out->addHTML("<table class='sortable wikitable' width='100%'>\n");
		$out->addHTML("<tr><th>LiPD Dataset</th><th class='unsortable' style='text-align:left'>");
		$out->addHTML("<a class='lodbutton x-small' href='?op=exportAll'>Download All</a>");
		if($del) {
			$out->addHTML("<a class='lodbutton x-small' onclick='return confirm(\"Are you sure you want to delete all datasets\");' ");
			$out->addHTML("href='?op=deleteAll'>Delete All</a>");
		}
		$out->addHTML("</th></tr>\n");
		$pageids = $this->getAllDatasets();
        foreach($pageids as $id) {
			$out->addHTML("<tr>\n");
			$out->addHTML("<td class='smwtype_wpg'><a href='$id'>$id</a></td>\n");
			$out->addHTML("<td>\n");
			$out->addHTML("<a class='lodbutton x-small' href='?op=export&lipdid=$id'>Download LiPD</a>\n");
			if($del) {
				$out->addHTML("<a class='lodbutton x-small' onclick='return confirm(\"Are you sure you want to delete \\n$id\");' ");
				$out->addHTML("href='?op=delete&lipdid=$id'>Delete</a>\n");
			}
			$out->addHTML("</td>\n");
			$out->addHTML("</tr>\n");
		}
		$out->addHTML("</table>\n");
	}

	function getGroupName() {
		return "ODSGroup";
	}

}
