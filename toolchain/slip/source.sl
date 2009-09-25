(fun sum-series (int:s int:e fun:f)
(if (= s e) 0
  (+ (f s) (sum-series (+ s 1) e f)))

(fun fact (int:n)
(if (= n 0) 1
  (* n (fact (- n 1)))))

(fun choose (int:n int:k)
(/ (fact n)
  (* (fact k) (fact (- n k)))))

;; btw, bind returns a copy of its argument in a piece of dynamically allocated memory - free does the opposite
(fun series-list (int:s int:e fun:f)
(if (= s e) (bind (cons (f s) nil))
  (bind (cons (f s) (series-list (+ s 1) e f))))

;; creates the list '(0 1 4 9 16 25)
(fun squares-to-five ()
(series-list (0 5 (n):(* n n))))
