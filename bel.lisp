
(def defun (syntax (name args . body)
             `(def ,name (lambda ,args ,@body))))

(def defmacro (syntax (name args . body)
                `(def ,name (syntax ,args ,@body))))

(defun id (a b)
  (eq a b))

(defun load (path)
  (load-file path))

(defun prn (x)
  (println x)
  x)
