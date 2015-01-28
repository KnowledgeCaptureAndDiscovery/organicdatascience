<?php
require_once 'Icon.php';
require_once 'Color.php';

abstract class MetaIcon extends Icon{
	
	private static $GRAY_Medium = '#e9e9e9';
	private static $GARY_DARK = '#cccccc';
	
	protected $size = 16;
	protected $border = 1;
	
   	public function __construct($fade, $warning) {
   		$this->fade = $fade;
   		$this->warning = $warning;
   	}
	
	protected function darkColor(){
		if($this->fade)
			return self::$GARY_DARK;
		if($this->warning)
			return Color::$YELLOW_DARK;
		return Color::$GREEN_DARK;		
	}
	
	protected function mediumColor(){
		if($this->fade)
			return self::$GARY_DARK;
		if($this->warning)
			return Color::$YELLOW_MEDIUM;
		return Color::$GREEN_MEDIUM;		
	}
	
	protected function lightColor(){
		if($this->fade)
			return Color::$GRAY_LIGHT;
		if($this->warning)
			return Color::$YELLOW_MEDIUM;
		return Color::$GREEN_LIGHT;	
	}
	
	protected function innerSize(){
		return $this->size-2*$this->border;
	}
	
	protected function cx(){
		return $this->size/2;
	}
	
	protected function cy(){
		return $this->size/2;
	}
	
}