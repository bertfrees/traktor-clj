(ns traktor.midi
  (:use [overtone.midi]))

(defn master-tempo
  ([]
    (let [tempo (promise)
          _ (master-tempo #(deliver tempo %))
          tempo (deref tempo 10000 nil)]
      (or tempo (throw (RuntimeException. "timeout")))))
  ([callback]
    (let [clock (midi-in (midi-find-device (midi-sources) "Traktor Virtual Output"))
          cancel #(let [transmitter (:transmitter clock)]
                    (.close (.getReceiver transmitter))
                    (.setReceiver transmitter nil))
          timestamps (ref ())]
      (midi-handle-events clock
        (fn [msg]
          (if (= (:status msg) :timing-clock)
            (if (> (count @timestamps) 100)
              (let [tempo (->> (- (:timestamp msg) (apply min @timestamps))
                               (/ (count @timestamps))
                               (* (/ 60000000 24))
                               float)]
                (callback tempo)
                (cancel))
              (dosync (alter timestamps conj (:timestamp msg)))))))
      cancel)))