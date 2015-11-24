<?php
require_once 'Icon.php';
require_once 'Color.php';

class TaskIcon extends Icon{
	
	public function __construct($progress, $sizeType, $fade, $warning, $childWarning){
		$this->sizeType = $sizeType;		
		$this->fade = $fade;
		$this->warning = $warning;
		$this->childWarning = $childWarning;
		$this->initProgress($progress);	
		$this->initBorder();		
		$this->initRadius();
	}
	
	public function metaprogress(){
		$this->meta = true;
		$this->radius-=.5;
	}
	
	public function progress($type, $overdue, $childoverdue){			
		$this->meta = false;
		$this->type = $type;	
		$this->overdue = $overdue;
		$this->childoverdue = $childoverdue;
				
		if($this->progress < 0) throw new Exception('Percentage value p minimum 0!');
		if($this->progress > 1) throw new Exception('Percentage value p maximum 1!');
		if($this->type != 'low' && $this->type != 'medium' && $this->type != 'high') throw new Exception('Type must be defined');
  	}
  	
	/** Cicle is not drawable for 0% and 100% Fix*/
	private function initProgress($progress){
		if($progress == 0) 
			$progress = 0.0001;
		if($progress == 1)
			$progress = 0.9999;
		$this->progress = $progress;
	}
  	
   	private function initBorder(){
  		switch ($this->sizeType) {
			case 'large': 	$this->border = 1; break;
			case 'medium': 	$this->border = 2; break;
			case 'small': 	$this->border = 1; break;
			default: throw new Exception('Border not specified for size!');
		}
  	}
  	
  	private function initRadius(){
  		switch ($this->sizeType) {
			case 'large': 	$this->radius = 14; break;
			case 'medium': 	$this->radius = 10; break;
			case 'small': 	$this->radius = 5.5; break;
			default: throw new Exception('Radius not specified for size!');
		}
  	}  	

	protected function renderSvg(){			
		$svg  = $this->calcProgressPie();
		$svg .= $this->calcProgressLabel();
		return $svg;
	}
	
	private function calcProgressPie(){
		$dangle = $this->progress*pi()*2;
		$dDone = $this->calcPathParamD(0, $dangle);
		$dLeft = $this->calcPathParamD($dangle, 1*pi()*2);
		if($this->meta){
			$pie  = '<path d="'.$dDone.'" stroke="'.$this->doneColor().'" stroke-width="2" fill="transparent"/>';
			$pie .= '<path d="'.$dLeft.'" stroke="'.$this->leftColor().'" stroke-width="2" fill="transparent"/>';			
		}else{
			$pie  = '<path d="'.$dDone.'" fill="'.$this->doneColor().'"/>';
			$pie .= '<path d="'.$dLeft.'" fill="'.$this->leftColor().'"/>';		
			if($this->childoverdue && !$this->overdue && $this->sizeType != 'small')
				$pie .= $this->childOverdueSymbol();
		}
		if($this->childWarning && !$this->warning)
			$pie .= $this->childWarningSymbol();
		return $pie;
	}
	
	private function childOverdueSymbol(){
		$cx = $this->radius*1.75;
		$r = $this->radius/3;
		if($this->fade)
			$c = Color::$GRAY_DARK;
		else 
			$c = Color::$ORANGE_DARK;
		return '<circle cx="'.$cx.'" cy="'.$cx.'" r="'.$r.'" fill="'.$c.'" />';		
	}
	
	private function childWarningSymbol(){
		$cx = $this->border;
		$cy = $this->radius*2+$this->border;
		$width = (2/5)*$this->radius*2;
		if($this->fade)
			$c = Color::$GRAY_DARK;
		else 
			$c = Color::$YELLOW_DARK;
		return '<path d="M '.$cx.' '.$cy.' l '.$width.' 0 l -'.($width/2).' -'.$width.' z" fill="'.$c.'" stroke="'.$c.'" />';		
	}
	
	/**
	 * Calculates the d parameter of the svg path element.
	 * @param float $startangle in degree
	 * @param float $endangle in degree
	 */
	private function calcPathParamD($startangle, $endangle){
		$cx = $this->radius+$this->border; 	// x-center-position pie
		$cy = $this->radius+$this->border; 	// y-center-position pie
		
		$x1 = $cx + $this->radius * sin($startangle);
		$y1 = $cy - $this->radius * cos($startangle);
		$x2 = $cx + $this->radius * sin($endangle);
		$y2 = $cy - $this->radius * cos($endangle);
				
		/** This is a flag for angles larger than than a half circle
		/*  It is required by the SVG arc drawing component */
		$big = 0;
		if ($endangle - $startangle > pi()) $big = 1;
				
		/** This string holds the path details */
		$r = $this->radius;
		$d = "M $x1, $y1";  		// Start at circle 
		$d .= " A $r, $r";      	// Draw an arc of radius r
		$d .= " 0 $big 1";      	// Arc details...
		$d .= "$x2, $y2";       	// Arc goes to to (x2,y2)
		if(!$this->meta){
			$d .= " L $cx, $cy";	// Draw line to cycle center
			$d .= " Z";             // Close path back to cycle border
		}		
		return $d;
	}
	
	private function calcProgressLabel(){		
		if($this->sizeType == 'large'){
			$r = $this->radius;
			$b = $this->border;
			$percentage = round($this->progress*100);
			$size = ($r*2)/2.7;
			$y = $r+$size/2+$b-1;
			if($percentage<10){
				$x = $r/2+$size/4+$b;
			}else if($percentage<100){
				$x = $r/2+$b;
			}else if($percentage==100){
				$x = $size/4+$b;
			}
			return '<text x="'.$x.'" y="'.$y.'" font-weight="bold" font-family="Verdana" font-size="'.$size.'"  style="text-anchor: middel; fill:'.$this->textColor().';" >'.$percentage.'</text>';
		}
	}
	
	private function doneColor(){
		if($this->fade)
			return Color::$GRAY_DARK;
		else
			if($this->warning)
				return Color::$YELLOW_DARK;
			else
				if($this->meta)
					return Color::$GREEN_MEDIUM;
				else			
					if($this->overdue)
						return Color::$ORANGE_DARK;
					else
						switch ($this->type) {
							case 'low': 	return Color::$GREEN_DARK;
							case 'medium': 	return Color::$GREEN_MEDIUM;
							case 'high': 	return Color::$GREEN_LIGHT;
							default: throw new Exception('Type does not exist');
						}
	}
	
	private function leftColor(){
		if($this->fade)
			return Color::$GRAY_LIGHT;
		else	
			if($this->warning)
				return Color::$YELLOW_LIGHT;
			else
				if($this->meta)
					return Color::$GRAY_LIGHT;
				else					
					if($this->overdue)
						return Color::$ORANGE_LIGHT;		
					else	
						return Color::$GRAY_LIGHT;		
	}
	
	private function textColor(){
		if($this->fade)
			return Color::$GRAY_DARKDARK;
		else
			if($this->warning)
				return Color::$YELLOW_DARKDARK;
			else
				if($this->meta)
					return Color::$GREEN_MEDIUM;
				else 
					if($this->overdue)
						return Color::$ORANGE_DARKDARK;
					else 
						return Color::$GREEN_DARKDARK;
	}	
}
