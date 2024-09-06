package main

import (
	"fmt"
	"io/ioutil"
	"os/exec"
	"path/filepath"
	"time"
)

// BenchmarkFile runs a file the specified number of times and returns the total duration
func BenchmarkFile(filePath string, iterations int) time.Duration {
	start := time.Now()

	for i := 0; i < iterations; i++ {
		cmd := exec.Command(filePath) // Assuming the file is executable
		err := cmd.Run()
		if err != nil {
			fmt.Printf("Error running file %s: %v\n", filePath, err)
			return 0
		}
		fmt.Println("MaxRSS:", cmd.ProcessState.SysUsage().(*syscall.Rusage).Maxrss)
// 		fmt.Printf("Iteration %d complete\n", i)
	}

	return time.Since(start)
}

func main() {
	dir := "./filesToRun" // Set the directory you want to benchmark files in

	files, err := ioutil.ReadDir(dir)
	if err != nil {
		fmt.Printf("Error reading directory: %v\n", err)
		return
	}

	iterations := []int{1} 
	//iterations := []int{10, 100, 1000, 10000} 

	for _, file := range files {
		if file.Mode().IsRegular() { // Only regular files
			filePath := filepath.Join(dir, file.Name())
			fmt.Printf("Benchmarking file: %s\n", filePath)

			for _, iter := range iterations {
				duration := BenchmarkFile(filePath, iter)
				fmt.Printf("Iterations: %d, Time: %v\n", iter, duration)
			}
		} else {
			fmt.Printf("Skipping file: %s\n", file.Name())
		}
	}
}
