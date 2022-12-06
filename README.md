# MPI Travelling Salesman

Considere um grafo $G$ como uma tupla $\left(V, A \right)$, sendo $V$ o conjunto de vértices desse grafo e, $A$, o seu conjunto de arestas. Desse modo, sendo $N = \left| V \right|$, pode-se enumerar os vértices como $\{ v_1, v_2, \dots, v_N \}$; considere, ainda, $adj(v_i)$, $i\in\left[1,N\right]\subset\N$, como o conjunto de vértices adjacentes ao nó $v_i$, isto é, $adj(v_i)=\{v_j \mid \exists \left( v_i, v_j \right) \in A \}$. Daí, dado um caminho $p$ contendo uma sequência de vértices, pode-se definir ${valid\_adj}_{p}(v_i) = \{ v_j \mid \exists \left( v_i, v_j \right) \in A  \land v_j \notin p \}$, isto é, o conjunto de vértices acessíveis a seguir que não pertençam ao caminho previamente percorrido, e $cost\left(p\right) = \sum_{i=1}^{\left| p \right|} c_{i-1, i}$, em que $c_{i,j}$ é o peso da aresta $\left( v_i, v_j \right)$. Por fim, considerando ${M}_{pv_i} = \left| {valid\_adj}_{p}(v_i) \right|$, propõe-se o seguinte grafo de dependências de tarefas:

```mermaid
%% Paralelização do problema do caixeiro viajante

%% Tipo de grafo
graph TD 

%% Inicialização
subgraph Begin
	id0[(N <-- read_data)] --> id1[G <-- generate_graph]
end

%% Enumeração dos caminhos
subgraph Enumerate

	%% Linha 1
	id1[G <-- generate_graph] --> v_1
	id1[G <-- generate_graph] --> v_2
	id1[G <-- generate_graph] --> id2{{...}}
	id1[G <-- generate_graph] --> v_N
	
	%% Linha 2 :: Vértice 1
	v_1 --> valid_adj_1
	v_1 --> valid_adj_2
	v_1 --> id3{{...}}
	v_1 --> valid_adj_M
	
	%% Linha 2 :: demais vértices
	v_2 --> id4{{...}}
	v_N --> id5{{...}}
	
	%% Linha 3
	valid_adj_1 --> id6{{...}}
	valid_adj_2 --> id7{{...}}
	valid_adj_M --> id8{{...}}
	id4{{...}} --> id9{{...}}
	id5{{...}} --> id10{{...}}
end

%% Custos totais dos caminhos
subgraph Get Path Costs
	id6{{...}} --> id11[p, cost_p]
	id7{{...}} --> id12[p, cost_p]
	id8{{...}} --> id13[p, cost_p]
	id9{{...}} --> id14[p, cost_p]
	id10{{...}} --> id15[p, cost_p]
end

%% Redução para o menor caminho
subgraph Reduction

	%% Linha 1
	id11[p, cost_p] --> id16(min)
	id12[p, cost_p] --> id16(min)
	id12[p, cost_p] --> id17(min)
	id13[p, cost_p] --> id17(min)
	id13[p, cost_p] --> id18(min)
	id14[p, cost_p] --> id18(min)
	id14[p, cost_p] --> id19(min)
	id15[p, cost_p] --> id19(min)
	
	%% Linha 2
	id16(min) --> id20(min)
	id17(min) --> id20(min)
	id17(min) --> id21(min)
	id18(min) --> id21(min)
	id18(min) --> id22(min)
	id19(min) --> id22(min)
	
	%% Linhas 3 e 4
	id20(min) --> id23(min)
	id21(min) --> id23(min)
	id21(min) --> id24(min)
	id22(min) --> id24(min)
	id23(min) --> id25(min)
	id24(min) --> id25(min)
end

%% Finalização
subgraph End
	id25(min) --> id26[p*, cost_p*]
end
```
