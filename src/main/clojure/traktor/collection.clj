(ns traktor.collection
  (:use [clj-xpath.core])
  (:require [clojure.java.io :as io]
            [clojure.string :as string]))

(def ^:private collection (xml->doc (io/input-stream (str (System/getProperty "user.home") "/Music/Traktor/collection.nml"))))

;; FIXME: escape quotes
(defn find-tracks [{title :title artist :artist filename :filename dir :dir}]
  (if (not (every? nil? [title artist filename dir]))
    (let [query (str "/NML/COLLECTION/ENTRY[not(LOOPINFO)"
                     (if title (str " and @TITLE='" title "'") "")
                     (if artist (str " and @ARTIST='" artist "'") "")
                     (if filename (str " and LOCATION/@FILE='" filename "'") "")
                     (if dir (str " and LOCATION/@DIR='" (string/replace dir #"/" ":/") "'") "")
                     "]")]
      (for [entry ($x query collection)]
        (let [location (first ($x "LOCATION" (:node entry)))
              info (first ($x "INFO" (:node entry)))
              tempo (first ($x "TEMPO" (:node entry)))]
          {:title (:TITLE (:attrs entry))
           :artist (:ARTIST (:attrs entry))
           :file (string/replace (str (:DIR (:attrs location)) (:FILE (:attrs location))) #"/:" "/")
           :tempo (if tempo (if-let [bpm (:BPM (:attrs tempo))] (Float. bpm)))
           :key (if info (if-let [key (:KEY (:attrs info))] (keyword key)))})))))
