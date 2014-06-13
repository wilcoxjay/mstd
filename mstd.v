Require Import List.
Import ListNotations.
Require Import ListSet.
Require Import ZArith.

Require Import Omega.

Require Import CorgiTactics.

Open Scope Z_scope.

Fixpoint sumset (A : set Z) : set Z := 
  match A with
    | [] => []
    | a :: A' => set_union Z.eq_dec (map (fun n => a + n) (a :: A')) (sumset A')
  end.

Theorem sumset_sound : forall A a b,
                         In a A ->
                         In b A ->
                         In (a + b) (sumset A).
Proof.
  induction A; intros.
  - solve_by_inversion.
  - simpl in *. intuition; subst; subst; simpl.
    + apply set_union_intro1. simpl. auto. 
    + apply set_union_intro1. right. apply in_map_iff. eexists; intuition.
    + apply set_union_intro1. right. apply in_map_iff. eexists; intuition.
    + apply set_union_intro2. apply IHA; auto.
Qed.

Theorem sumset_complete : forall A x,
                            In x (sumset A) ->
                            exists a b,
                              In a A /\
                              In b A /\
                              x = a + b.
Proof.
  induction A; intros.
  - solve_by_inversion.
  - simpl in *. apply set_union_elim in H.
    simpl in *. intuition.
    + eauto 10.
    + apply in_map_iff in H. break_exists. intuition eauto 10.
    + apply IHA in H0. break_exists. intuition eauto 10.
Qed.

Fixpoint diffset (A : set Z) : set Z :=
  match A with
    | [] => []
    | a :: A' => set_union Z.eq_dec (map (fun n => a - n) (a :: A'))
                (set_union Z.eq_dec (map (fun n => n - a) (a :: A'))
                           (diffset A'))
  end.

Theorem diffset_sound : forall A a b,
                          In a A ->
                          In b A ->
                          In (a - b) (diffset A).
Proof.
  induction A; intros.
  - solve_by_inversion.
  - simpl in *. intuition; subst; subst; simpl in *.
    + apply set_union_intro1. simpl. auto.
    + apply set_union_intro1. right. apply in_map. auto.
    + apply set_union_intro2. apply set_union_intro1. right. apply in_map_iff. eauto.
    + apply set_union_intro2. apply set_union_intro2. apply IHA; auto.
Qed.

Theorem diffset_complete : forall A x,
                             In x (diffset A) ->
                             exists a b,
                               In a A /\
                               In b A /\
                               x = a - b.
Proof.
  induction A; intros.
  - solve_by_inversion.
  - simpl in *. apply set_union_elim in H. simpl in *.
    intuition.
    + exists a, a. eauto.
    + apply in_map_iff in H. break_exists. exists a, x0. intuition.
    + apply set_union_elim in H0. simpl in *. intuition.
      * exists a, a. eauto.
      * apply in_map_iff in H0. break_exists. exists x0, a. intuition.
      * apply IHA in H. break_exists. exists x0, x1. intuition.
Qed.
      
Section normalize.
  Variable A : Type.
  Hypothesis A_eq_dec : forall (a b : A), {a = b} + {a <> b}.

  Fixpoint normalize (s : set A) : set A :=
    match s with
      | [] => []
      | x :: s' => set_add A_eq_dec x (normalize s')
    end.

  Theorem normalize_contents : forall s x,
                                 In x s <-> In x (normalize s).
  Proof.
    induction s; intros; split; intros.
    - solve_by_inversion.
    - solve_by_inversion.
    - simpl in *. intuition.
      + subst. apply set_add_intro2. auto.
      + apply set_add_intro1. apply IHs. auto.
    - simpl in *. apply set_add_elim in H. intuition.
      apply IHs in H0. auto.
  Qed.

  Lemma set_add_In_noop : forall s x,
                            In x s ->
                            set_add A_eq_dec x s = s.
  Proof.
    induction s; intros.
    - solve_by_inversion.
    - simpl in *. intuition.
      + subst. break_if; congruence.
      + break_if; f_equal; auto.
  Qed.

  Lemma set_add_not_In_cons : forall s x,
                                ~ In x s ->
                                set_add A_eq_dec x s = s ++ [x].
  Proof.
    induction s; intros.
    - auto.
    - simpl. break_if.
      + subst. exfalso. intuition.
      + f_equal. apply IHs. intuition.
  Qed.


  Hint Constructors NoDup.
  Lemma NoDup_snoc : forall (s : list A),
                       NoDup s ->
                       forall x,
                         ~ In x s -> 
                         NoDup (s ++ [x]).
  Proof.
    intros s H.
    induction H; intros.
    - simpl. auto.
    - simpl in *.
      intuition.
      pose proof IHNoDup _ H3.
      constructor; auto. intro.
      apply in_app_or in H4. intuition. inv H5; auto.
  Qed.
    

  Theorem normalize_no_dup : forall s,
                               NoDup (normalize s).
  Proof.
    induction s.
    - constructor.
    - simpl. pose proof in_dec A_eq_dec a (normalize s). intuition.
      + rewrite set_add_In_noop; auto.
      + rewrite set_add_not_In_cons; auto. apply NoDup_snoc; auto.
  Qed.
End normalize.


Require Import NPeano.

Delimit Scope nat_scope with nat_scope.

Definition is_mstd (A : set Z) := (length (diffset A) <? length (sumset A))%nat_scope.

Fixpoint power (A : set Z) : set (set Z) :=
  match A with
    | [] => [[]]
    | x :: A' => flat_map (fun s => [ x :: s ; s ]) (power A')
  end.

Require Import Coq.Program.Wf.

Require Import Recdef.

Function range (p : Z * Z) {measure (fun p : Z*Z =>
                                     let (a, b) := p in
                                     if Z_lt_dec b a then 0%nat else S (Z.abs_nat (b - a)))
                                  p} :=
  let (a, b) := p in
  if Z_lt_dec b a then []
  else a :: range ((a + 1), b).
- intros.
  repeat break_if.
  + contradiction.
  + omega.
  + contradiction.
  + apply lt_n_S. apply Zabs_nat_lt. omega.
Defined.

Definition count_mstd (n : Z) := length (filter is_mstd (power (range (1,n)))).

Fixpoint pos_interp (z : Z) (s : positive) : set Z :=
  match s with
    | xH => [z]
    | xI p' => z :: pos_interp (z + 1) p'
    | xO p' => pos_interp (z + 1) p'
  end.


Fixpoint Z_interp (s : Z) : set Z :=
  match s with
    | Zpos p => pos_interp 0 p
    | _ => []
  end.

Eval compute in (map Z_interp (range (0, Z.pow_pos 2 2 - 1))).
Eval simpl in (power (range (0, 1))).

Theorem bitset :
  forall n, map Z_interp (range (0, Z.pow_pos 2 n - 1)) = power (range (0, Zpos n - 1)).














Extraction Language Haskell.

Extract Inductive bool => "Bool" ["True" "False"].
Extract Inductive nat => "Int" ["0" "(\x -> x + 1)"]
                                       "(\z s n -> if n == 0 then z () else s (n - 1))".
Extract Inlined Constant negb => "not".

Extract Inlined Constant fst => "fst".
Extract Inlined Constant snd => "fst".

Extract Inductive prod => "(,)" ["(,)"] "(\f (x,y) -> f x y)".

Extract Inductive list => "[]" ["[]" "(:)"] "(\n c l -> if null l then n () else c (head l) (tail l))".
Extract Inlined Constant length => "length".
Extract Inlined Constant app => "(++)".
Extract Inlined Constant map => "map".
Extract Inlined Constant flat_map => "concatMap".
Extract Inlined Constant filter => "filter".


Extract Inductive sumbool => "Bool" ["True" "False"].

Extract Inductive comparison => "Ordering" ["EQ" "LT" "GT"].


Extraction "count_mstd.hs" count_mstd.
