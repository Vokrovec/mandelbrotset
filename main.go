package main

import (
	"math/cmplx"
	"image"
	"image/color"
	"image/png"
	"os"
	"fmt"
)

func main()  {
	width := 10000 
	height := 10000 
	grid :=	mandelbrot(height, width, 255) 
	img := makeImage(grid, height, width)	
	f, _ := os.Create("image.png")
	png.Encode(f, img)
}

func makeImage(grid [][]uint8, height int, width int) image.Image {
	upLeft := image.Point{0, 0}
	downRight := image.Point{height, width}
	img := image.NewRGBA(image.Rectangle{upLeft, downRight})
	for i:=0; i<height; i++{
		for j:=0; j<width; j++{
			clr := color.RGBA{0, 255-grid[i][j], grid[i][j], 255}	
			if grid[i][j] >= 255{
				img.Set(i, j, color.Black)
			} else{
				img.Set(i, j, clr) //sets color of pixel by its max itteration 
			}
		}
	}
	return img
}

func mandelbrot(height int, width int, max_iter int) [][]uint8{
	var px_size float64 = 3/float64(width)
	grid := make([][]uint8, height) // making grid of pixels, that will store max itteration
	var x float64 = -2  
	d := calc_circle(px_size, max_iter)
	for i:=0; i<height; i++ {
		var y float64 = -1.5
		grid[i] = make([]uint8, width)
		for j:=0; j<width; j++{
			y += px_size
			if x < d && y < d && x > -d && y > -d{
				grid[i][j] = uint8(max_iter)
			}	else {
				grid[i][j] = validate_point(x , y, max_iter)
			}	
		}
		x += px_size
		fmt.Println(x)
	}
	return grid
}

func calc_circle(px_size float64, max_iter int) float64{ //calculate circle with center in point 0, 0 (all points in will be true)
	var x float64 = 0 
	for {
		if validate_point(x, 0, max_iter) != uint8(max_iter){
			break
		} 
		x += px_size
	}
	return x // returns diameter of circle
}
func validate_point(x float64, y float64, max_iter int) uint8{
	c := complex(x, y)
	z := c*c + c
	for i:=0; i <= max_iter; i++{
		if cmplx.Abs(z) >= 2{
			return uint8(i)	
		}
		z = z*z + c
	}	
	return uint8(max_iter)
}
