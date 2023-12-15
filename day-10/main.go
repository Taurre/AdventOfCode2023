package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
)

type Coord struct {
	X, Y int
}

var (
	Pipes = map[rune][2]Coord{
		'|': {{X: -1, Y: 0}, {X: 1, Y: 0}},
		'-': {{X: 0, Y: -1}, {X: 0, Y: 1}},
		'L': {{X: -1, Y: 0}, {X: 0, Y: 1}},
		'J': {{X: -1, Y: 0}, {X: 0, Y: -1}},
		'7': {{X: 1, Y: 0}, {X: 0, Y: -1}},
		'F': {{X: 1, Y: 0}, {X: 0, Y: 1}},
	}
	Moves = [4]Coord {
		{X: 0, Y: -1},
		{X: -1, Y: 0},
		{X: 0, Y: 1},
		{X: 1, Y: 0},
	}
)

func main() {
	var err error
	input := os.Stdin
	log.SetFlags(0)

	if len(os.Args) > 1 {
		input, err = os.Open(os.Args[1])

		if err != nil {
			log.Fatalf("Could not open %s: %v\n", os.Args[1], err.Error())
		}
	}

	scanner := bufio.NewScanner(input)
	sketch := make(map[Coord]rune)
	line := 0
	var start Coord

	for scanner.Scan() {
		for col, tile := range scanner.Text() {
			sketch[Coord{X: line, Y: col}] = tile

			if tile == 'S' {
				start = Coord{X: line, Y: col}
			}
		}

		line++
	}

	if err := scanner.Err(); err != nil {
		log.Fatalf("scanner.Scan(): %v\n", err.Error())
	}

	current := start
	loop := make([]Coord, 0)
	visited := make(map[Coord]bool)
	visited[start] = true
	loop = append(loop, start)

	for _, move := range Moves {
		next := Coord{X: start.X+move.X, Y: start.Y+move.Y}

		if pipe, ok := sketch[next]; ok && pipe != '.' {
			for _, move := range Pipes[pipe] {
				if visited[Coord{X: next.X+move.X, Y: next.Y+move.Y}] {
					current = next
					visited[current] = true
					loop = append(loop, current)
					goto route
				}
			}
		}
	}
route:
	for _, move := range Pipes[sketch[current]] {
		next := Coord{X: current.X+move.X, Y: current.Y+move.Y}

		if pipe, ok := sketch[next]; ok {
			if pipe == '.' {
				continue
			}
			if !visited[next] {
				current = next
				visited[current] = true
				loop = append(loop, current)
				goto route
			}
		}
	}

	fmt.Printf("Part 1: %d\n", len(loop) / 2)
	count := 0

	for coord, _ := range sketch {
		enclosed := true

		if visited[coord] {
			continue
		}

		for _, move := range Moves {
			next := Coord{X: coord.X+move.X, Y: coord.Y+move.Y}

			if _, ok := sketch[next]; !ok {
				enclosed = false
				break;
			}
			if !visited[next] {
				enclosed = false
				break;
			}
		}

		if enclosed {
			visited[coord] = true
			count++
		}
	}

	fmt.Printf("Part 2: %d\n", count)
}
