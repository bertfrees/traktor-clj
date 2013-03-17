[traktor-clj][]
===============
A Clojure library for interacting with [Traktor][].

Usage
-----
Include `traktor-clj` in your `:dependencies` like this:

    [traktor-clj "0.1.0-SNAPSHOT"]

and add my snapshot repository to your custom `:repositories`

    ["bertfrees-github-snapshots" "http://bertfrees.github.com/snapshots"]

### Master tempo
For this to work, first [set up Traktor](http://www.native-instruments.com/knowledge/questions/817/How+to+send+a+MIDI+clock+sync+signal+from+TRAKTOR%3F) to send a MIDI clock signal.

    (use 'traktor.midi)
    (master-tempo)

License
-------
Copyright © 2013 [Bert Frees][bert]

Distributed under the Eclipse Public License, the same as Clojure.

[traktor-clj]: http://github.com/traktor-clj
[traktor]: http://www.native-instruments.com/en/traktor/
[bert]: http://github.com/bertfrees