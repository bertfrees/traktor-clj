[traktor-clj][]
===============
A Clojure library for interacting with [Traktor][].

Usage
-----
Include `traktor-clj` in your `:dependencies` like this:

```clojure
[traktor-clj "0.1.0-SNAPSHOT"]
```

and add my snapshot repository to your custom `:repositories`

```clojure
["bertfrees-github-snapshots" "http://bertfrees.github.com/snapshots"]
```

### Master tempo
For this to work, first [set up Traktor](http://www.native-instruments.com/knowledge/questions/817/How+to+send+a+MIDI+clock+sync+signal+from+TRAKTOR%3F) to send a MIDI clock signal.

```clojure
user> (use 'traktor.midi)
user> ;; get a value after blocking for at most 10 seconds:
user> (master-tempo)
127.99894
user> ;; get a reference whose value changes constantly:
user> (def tempo (master-tempo-agent))
user> @tempo
127.999886
user> @tempo
128.0006
user> ;; watch the tempo change:
user> (def cncl (master-tempo-watch println))
128.00177
128.00177
127.99958
user> (cncl)
user> ;; only get notified of tempo changes above 0.5 BPM:
user> (master-tempo-watch println 0.5)
```

License
-------
Copyright © 2013 [Bert Frees][bert]

Distributed under the Eclipse Public License, the same as Clojure.

[traktor-clj]: http://github.com/traktor-clj
[traktor]: http://www.native-instruments.com/en/traktor/
[bert]: http://github.com/bertfrees