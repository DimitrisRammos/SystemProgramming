Δημητριοσ Ραμμος
ΑΜ: 1115201900161

Στο φακελο program υπαρχουν 2 αρχεια κωδικα και ενα Makefile.

Server -> dataServer.cpp
Client -> remoteClient.cpp

Για τον dataServer.cpp:

Δημιουργειται ενα σοκετ για με το οποιο επικοινωνουν οι διαφοροι remoteClient.
Ο  Server  για καθε νεα του συνδεση δημιουργει ενα communication thread, το οποιο διαβαζει
το ονομα του φακελου που επιθυμει ο Client για αντιγραφη. Επειτα ο Server βρισκει ολα τα αρχεια του 
φακελου αυτου ή και των υποφακελων του και τα απθηκευει σε μια ουρα ως pathname μαζι με το socket number.
Φυσικα στην ουρα αποθηκευονται τοσα οσα χωρανε απο την εισοδο που δινεται στην εκτελεση του προγραμματος.

Ο Server δημιουργει Workers threads τοσους οσους το thread_pool_size.
Οι threads αυτοι διαβαζουν απο την ουρα ενα pathname & socket και κλειδωνουν το mutex,  ωστε να μην γραφει με αλλο παραλληλα.
Μετα ο worker αυτος δημιουργει μια συμβολοσειρα απο pathname, block_size & file_size και τα δινει στον Client μεσω του socket.
Επειτα ο Worker παιρνα το περιεχομενο του αρχειου και αντιστοιχα ο client το γραφει στο αρχειο που δημιοργηθηκε.

Για το remoteClient.cpp:

Ανοιγουμε το σοκετ που εχει δημιουργηθει μεσω των ορισματων και δινουμε στο Server κανοντας του ετοιμα το ονομα του καταληγουμε που θελουμε.
Ο client  μετα αντιγραφει στο αρχειο τις πληροφοριες που του δινει ο worker.

Για παραδειγμα αν ο Client δωσει το directory με ονομα Server, o καταλογος αυτος μαζι με υποκαταλογους του και τα σχετικα αρχεια θα γινουν αντιγραφη αλλα 
θα απθηκευτουν ολα στο φακελο με ονομα out. The directory out ειναι στον ιδιο καταλογο με τα αρχεια μας.

Στην αρχη ο client  καθε φορα που θα γραφει καποιος worker θα δεχεται ενα buffer που θα χει pathname, block_size & file_size και θα κανει parsing ωστε να 
τα ξεχωριζει.
Επειτα αν χρειαστει θα δημιουργει καταλογους και το αρχειο αφου κανει διαγραφη αν υπαρχει κατι παλαιοτερο και επειτα θα γραφει τα δεδομενα του απο τον worker.



Για να τρεξετε το προγραμμα, ανοιξτε ενα τερμιναλ για το Server και οσα θελετε για client.

Terminal Server:
make run_server

Terminal:
make run_client

Για να διακοωετε την λειτουργια καποιου τερμιναλ πατηστε ctrl + C.