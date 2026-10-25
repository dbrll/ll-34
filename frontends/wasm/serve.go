// Serve the ll-34 browser version on localhost.
//
// usage: go run frontends/wasm/serve.go [port]   (default 1134)
package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"runtime"
)

func main() {
	port := "1134"
	if len(os.Args) > 1 {
		port = os.Args[1]
	}
	_, self, _, _ := runtime.Caller(0)
	files := http.FileServer(http.Dir(filepath.Dir(self)))
	http.Handle("/", http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Cache-Control", "no-cache")
		files.ServeHTTP(w, r)
	}))
	fmt.Printf("ll-34 running at http://localhost:%s\n", port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}
