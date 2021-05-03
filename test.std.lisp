
(defun write-utf8 (file code)
  (cond
    ((< code 128)
     (write-u8 file code))

    (t
     (error "not implemented"))))

(let ((file (fopen "test.txt" "wb")))
  (write-utf8 file (ord "A"))
  (write-utf8 file (ord "b"))
  (write-utf8 file (ord "r"))
  (write-utf8 file (ord "a"))
  (write-utf8 file (ord "c"))
  (write-utf8 file (ord "a"))
  (write-utf8 file (ord "d"))
  (write-utf8 file (ord "a"))
  (write-utf8 file (ord "b"))
  (write-utf8 file (ord "r"))
  (write-utf8 file (ord "a"))
  (write-utf8 file (ord "\n"))
  (fclose file))

;;(def std (make-env *env*))
;;
;;(with-env std
;;  (load-file "std.lisp"))
;;
;;(println 'pi: (with-env std pi))

(load-file "std.lisp")

(defun test-report (exp val res)
  (if (equal val res)
      (println (intern "\x1b[32mPASS\x1b[0m") exp '=> res)
      (println (intern "\x1b[31mFAIL\x1b[0m") exp '=> val '!= res)))

(defmacro test (exp _ res)
  `(test-report ',exp ,exp ',res))

(test nil => nil)
(test t => t)

(test 'foo => foo)

(test (if nil 'a 'b) => b)
(test (if 'eq 'a 'b) => a)

(test (equal '(a . b) '(a . b)) => t)

(test (cons 'a 'b) => (a . b))

(test (car (cons 'a 'b)) => a)
(test (cdr (cons 'a 'b)) => b)

(test (car '(a b c)) => a)
(test (cdr '(a b c)) => (b c))

(test (car '(a b c)) => a)
(test (cadr '(a b c)) => b)
(test (caddr '(a b c)) => c)

;;; ord

(test (ord "A") => 65)
(test (ord "Ä") => 196)
(test (ord "あ") => 12354)
(test (ord "𐍈") => 66376)

;;; chr

(test (chr 65) => "A")
(test (chr 196) => "Ä")
(test (chr 12354) => "あ")
(test (chr 66376) => "𐍈")
