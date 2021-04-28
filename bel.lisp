
(def defun (syntax (name args . body)
             `(def ,name (lambda ,args ,@body))))

(def defmacro (syntax (name args . body)
                `(def ,name (syntax ,args ,@body))))

(def xar rplaca)

(def xdr rplacd)

(defun id (a b)
  (eq a b))

(defun load (path)
  (load-file path))

(defun prn (x)
  (println x)
  x)

(defun type (x)
  'symbol) ; TODO

(def join cons)
