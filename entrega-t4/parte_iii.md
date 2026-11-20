**Parte III**

**Estrutura de dados**  
**geracao.c (vetores de tamanho fixo)**

- problema: a geração guarda indivíduos e espécies em vetores de tamanho fixo (N\_INDIVÍDUOS e MAX\_ESPÉCIES), definidos em config.h.  
- motivo: se precisar de mais espaço, o programa para num assert, e para mudar o limite é preciso editar config.h. O vetor de espécies também reserva 500 posições mesmo quando há bem menos espécies.  
- implementação alternativa: vetores que crescem conforme necessário: começam pequenos e dobram de tamanho quando enchem.  
- por que melhora: não há mais limite fixo, e a memória usada acompanha o que realmente existe.

**geracao.c (indivíduos guardados em dois lugares)**

- problema: cada indivíduo está em dois vetores ao mesmo tempo: o da geração e o da sua espécie.  
- motivo: os dois precisam ser mantidos iguais à mão, e cada um é ordenado separadamente. A liberação de memória só funciona por combinação (a geração libera, a espécie não), e nada no código garante isso.  
- implementação alternativa: guardar os indivíduos só dentro das espécies. A geração guarda apenas as espécies.  
- por que melhora: cada indivíduo tem um único dono, com um só lugar para inserir e liberar.

**geracao.c (qsort repetido)**

- problema: a cada geração, os mesmos vetores são ordenados por completo várias vezes.  
- motivo: em vários pontos só se quer saber o melhor elemento (para gravar, imprimir e entregar as sobras), e ordenar tudo é trabalho a mais.  
- implementação alternativa: para achar só o melhor, uma passada pelo vetor guardando o maior (ou uma fila de prioridade). Onde a ordem completa é necessária, ordenar uma vez só por geração.  
- por que melhora: acha o melhor sem ordenar tudo, e as ordenações repetidas deixam de ser feitas.

**individuo.c (tabela de inovações)**

- problema: as inovações ficam num vetor de tamanho fixo (10000), e a procura é feita lendo a tabela inteira, uma por uma.  
- motivo: a tabela cresce a cada geração, então a procura fica cada vez mais lenta. Quando o vetor enche, o código só imprime um erro e segue sem registrar a inovação.  
- implementação alternativa: guardar as inovações numa árvore binária de busca, com o par (origem, destino) como chave.  
- por que melhora: a busca vai direto ao ponto, sem ler tudo, e como os nós são criados conforme necessário, o limite deixa de existir.

**individuo.c (busca de neurônio por id)**

- problema: para achar um neurônio pela id, o código lê todos os neurônios da rede até encontrar.  
- motivo: isso é feito dentro de laços, uma vez para cada sinapse comparada, e a comparação de indivíduos acontece centenas de vezes por geração.  
- implementação alternativa: cada indivíduo mantém uma árvore binária de busca, com a id do neurônio como chave e a posição dele na rede como valor.  
- por que melhora: a busca não precisa mais ler todos os neurônios.

**individuo.c (sorteio de sinapse por tentativas)**

- problema: para sortear uma sinapse existente, o código sorteia pares de neurônios e confere se há sinapse entre eles, repetindo até 50 vezes.  
- motivo: a rede tem poucas sinapses perto do total de pares possíveis, então a maioria das tentativas falha. Se as 50 acabarem, a mutação simplesmente não acontece.  
- implementação alternativa: cada indivíduo guarda uma lista (vetor que cresce) com suas sinapses, e a mutação sorteia direto uma posição dela.  
- por que melhora: o sorteio sempre acerta uma sinapse existente, e nenhuma mutação é perdida.

**especie.c (vetor de tamanho fixo)**

- problema: cada espécie guarda seus indivíduos num vetor de tamanho fixo (500 posições).  
- motivo: na prática, uma espécie tem poucos indivíduos, então quase todo o espaço fica vazio. Se uma espécie precisar de mais de 500, o programa para num assert.  
- implementação alternativa: um vetor que cresce conforme necessário: começa pequeno e dobra de tamanho quando enche.  
- por que melhora: cada espécie usa só a memória de que precisa, e o limite de 500 deixa de existir.

**rede.c (verificação de ciclo a cada sinapse inserida)**

- problema: rede\_insere\_sinapse insere a sinapse e depois verifica a rede inteira para ver se formou ciclo. Se formou, apaga a sinapse.  
- motivo: a verificação examina a rede toda, a cada sinapse. Quando há ciclo, a sinapse é criada e logo apagada, trabalho jogado fora.  
- implementação alternativa: antes de inserir, percorrer a rede a partir do nó destino. Se o percurso chegar ao nó origem, a sinapse criaria um ciclo e nem é inserida.  
- por que melhora: só visita os nós que o destino alcança, e nada é inserido para ser desfeito.

**API do grafo**  
**alteração em grafo.h**  
**Percorrer todas as arestas de uma vez**

- problema: hoje o grafo só mostra as arestas de um nó por vez. Para visitar todas as sinapses, o treinador precisa de dois laços, um pelos neurônios e outro pelas sinapses de cada um. Isso se repete em compatibilidade, indivíduo\_mistura\_sinapses, indivíduo\_grava e mutação\_altera\_valor\_sinapses.  
- sugestão: uma consulta que entregue, uma a uma, todas as arestas do grafo, com origem, destino e valor.  
- por que ajuda: cada uma dessas funções fica com um laço só.

**Saber quantas arestas existem e pegar a de posição k**

- problema: para sortear uma sinapse, mutação\_divide\_sinapse e mutação\_inverte\_habilitação\_sinapse sorteiam pares de neurônios até 50 vezes, torcendo para existir sinapse entre eles. Se não acham, a mutação é perdida.  
- sugestão: g\_num\_arestas, que informa o total, e uma função que devolve a aresta de posição k.  
- por que ajuda: basta sortear um número entre 0 e o total e pegar essa aresta, que sempre existe. O indivíduo não precisa manter uma lista própria das sinapses.

**Perguntar se existe caminho entre dois nós**

- problema: rede\_insere\_sinapse insere a sinapse e depois verifica o grafo inteiro para ver se formou ciclo. Se formou, apaga o que acabou de inserir.  
- sugestão: g\_existe\_caminho(origem, destino). Uma sinapse de A para B só forma ciclo se já houver caminho de B para A.  
- por que ajuda: a verificação acontece antes de inserir e olha só a parte da rede que importa, sem inserir e desfazer.