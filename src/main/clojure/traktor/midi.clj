(ns traktor.midi
  (:require [traktor.midi.native :as native]))

;; ------------ ;;
;; MASTER TEMPO ;;
;; ------------ ;;

(defn master-tempo-watch
  ([callback]
     (native/load)
     (let [native-callback (native/double-callback callback)
           native-cancel (native/master-tempo native-callback)]
       (fn [] (native/invoke-callback native-cancel)
         (native/discard-callback native-callback))))
  ([callback sensitivity]
     (if (> sensitivity 0)
       (let [tempo (atom 0)]
         (master-tempo-watch
          #(when (< sensitivity (Math/abs (- @tempo %)))
             (reset! tempo %)
             (callback %))))
       (master-tempo-watch callback))))

(defn master-tempo-agent []
  (let [tempo (agent nil)]
    (master-tempo-watch #(send tempo (constantly %)))
    tempo))

(defn master-tempo []
  (let [tempo (promise)
        cancel (master-tempo-watch #(deliver tempo %))
        tempo (deref tempo 10000 nil)]
    (cancel)
    (or tempo (throw (RuntimeException. "timeout")))))
